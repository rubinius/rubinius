require 'readline'
require 'debugger/frame'
require 'debugger/commands'
require 'debugger/breakpoint'
require 'debugger/display'

class Debugger
  include Debugger::Display

  ROOT_DIR = File.expand_path(File.dirname(__FILE__) + "/..")

  def initialize
    @file_lines = Hash.new do |hash, path|
      if File.exists? path
        hash[path] = File.readlines(path)
      else
        ab_path = File.join(@root_dir, path)
        if File.exists? ab_path
          hash[path] = File.readlines(ab_path)
        else
          hash[path] = []
        end
      end
    end

    @thread = nil
    @frames = []

    @variables = {
      :show_ip => false,
      :show_bytecode => false,
      :highlight => false
    }

    @loaded_hook = proc { |file|
      check_defered_breakpoints
    }

    @added_hook = proc { |mod, name, exec|
      check_defered_breakpoints
    }

    Rubinius::CodeLoader.loaded_hook.add @loaded_hook
    Rubinius.add_method_hook.add @added_hook

    @defered_breakpoints = []

    @user_variables = 0

    @breakpoints = []

    @history_path = File.expand_path("~/.rbx_debug")

    if File.exists?(@history_path)
      File.readlines(@history_path).each do |line|
        Readline::HISTORY << line.strip
      end
      @history_io = File.new(@history_path, "a")
    else
      @history_io = File.new(@history_path, "w")
    end

    @history_io.sync = true

    @root_dir = ROOT_DIR
  end

  attr_reader :variables, :current_frame, :breakpoints, :user_variables
  attr_reader :locations

  def self.global
    @global ||= new
  end

  def self.start
    global.start(1)
  end

  def self.here
    global.start(1)
  end

  def spinup_thread
    return if @thread

    @local_channel = Rubinius::Channel.new

    @thread = Thread.new do
      begin
        listen
      rescue Exception => e
        e.render("Listening")
        break
      end

      while true
        begin
          accept_commands
        rescue Exception => e
          begin
            e.render "Error in debugger"
          rescue Exception => e2
            puts "Error rendering backtrace in debugger!"
          end
        end
      end
    end

    @thread.setup_control!(@local_channel)
  end

  def start(offset=0)
    spinup_thread

    # Feed info to the debugger thread!
    locs = Rubinius::VM.backtrace(offset + 1, true)

    method = Rubinius::CompiledMethod.of_sender

    bp = BreakPoint.new "<start>", method, 0, 0
    channel = Rubinius::Channel.new

    @local_channel.send Rubinius::Tuple[bp, Thread.current, channel, locs]

    # wait for the debugger to release us
    channel.receive

    Thread.current.set_debugger_thread @thread
    self
  end

  def listen(step_into=false)
    if @channel
      if step_into
        @channel << :step
      else
        @channel << true
      end
    end

    # Wait for someone to stop
    bp, thr, chan, locs = @local_channel.receive

    # Uncache all frames since we stopped at a new place
    @frames = []

    @locations = locs
    @breakpoint = bp
    @debuggee_thread = thr
    @channel = chan

    @current_frame = frame(0)

    bp.hit! if bp

    puts
    info "Breakpoint: #{@current_frame.describe}"
    show_code

    if @variables[:show_bytecode]
      decode_one
    end

  end

  def accept_commands
    cmd = Readline.readline "debug> "

    if cmd.empty?
      cmd = @last_command
    else
      @last_command = cmd
    end

    command, args = cmd.strip.split(/\s+/, 2)

    runner = Command.commands.find { |k| k.match?(command) }

    if runner
      runner.new(self).run args
    else
      puts "Unrecognized command: #{command}"
      return
    end

    # Save it to the history.
    @history_io.puts cmd
  end

  def eval_code(args)
    obj = @current_frame.run(args)

    idx = @user_variables
    @user_variables += 1

    str = "$d#{idx}"
    Rubinius::Globals[str.to_sym] = obj
    puts "#{str} = #{obj.inspect}\n"
  end

  def frame(num)
    @frames[num] ||= Frame.new(self, num, @locations[num])
  end

  def set_frame(num)
    @current_frame = frame(num)
  end

  def each_frame(start=0)
    start = start.number if start.kind_of?(Frame)

    start.upto(@locations.size-1) do |idx|
      yield frame(idx)
    end
  end

  def set_breakpoint_method(descriptor, method, line=nil)
    exec = method.executable

    unless exec.kind_of?(Rubinius::CompiledMethod)
      error "Unsupported method type: #{exec.class}"
      return
    end

    if line
      ip = exec.first_ip_on_line(line)

      if ip == -1
        error "Unknown line '#{line}' in method '#{method.name}'"
        return
      end
    else
      line = exec.first_line
      ip = 0
    end

    bp = BreakPoint.new(descriptor, exec, ip, line)
    bp.activate

    @breakpoints << bp

    info "Set breakpoint #{@breakpoints.size}: #{bp.location}"

    return bp
  end

  def delete_breakpoint(i)
    bp = @breakpoints[i-1]

    unless bp
      error "Unknown breakpoint '#{i}'"
      return
    end

    bp.delete!

    @breakpoints[i-1] = nil
  end

  def add_defered_breakpoint(klass_name, which, name, line)
    dbp = DeferedBreakPoint.new(self, @current_frame, klass_name, which, name,
                                line, @defered_breakpoints)
    @defered_breakpoints << dbp
    @breakpoints << dbp
  end

  def check_defered_breakpoints
    @defered_breakpoints.delete_if do |bp|
      bp.resolve!
    end
  end

  def send_between(exec, start, fin)
    ss   = Rubinius::InstructionSet.opcodes_map[:send_stack]
    sm   = Rubinius::InstructionSet.opcodes_map[:send_method]
    sb   = Rubinius::InstructionSet.opcodes_map[:send_stack_with_block]

    iseq = exec.iseq

    fin = iseq.size if fin < 0

    i = start
    while i < fin
      op = iseq[i]
      case op
      when ss, sm, sb
        return exec.literals[iseq[i + 1]]
      else
        op = Rubinius::InstructionSet[op]
        i += (op.arg_count + 1)
      end
    end

    return nil
  end

  def show_code(line=@current_frame.line)
    path = @current_frame.method.active_path

    if str = @file_lines[path][line - 1]
      if @variables[:highlight]
        fin = @current_frame.method.first_ip_on_line(line + 1)
        name = send_between(@current_frame.method, @current_frame.ip, fin)

        if name
          str = str.gsub name.to_s, "\033[0;4m#{name}\033[0m"
        end
      end
      info "#{line}: #{str}"
    else
      show_bytecode(line)
    end
  end

  def decode_one
    ip = @current_frame.ip

    meth = @current_frame.method
    partial = meth.iseq.decode_between(ip, ip+1)

    partial.each do |ins|
      op = ins.shift

      ins.each_index do |i|
        case op.args[i]
        when :literal
          ins[i] = meth.literals[ins[i]].inspect
        when :local
          if meth.local_names
            ins[i] = meth.local_names[ins[i]]
          end
        end
      end

      display "ip #{ip} = #{op.opcode} #{ins.join(', ')}"
    end
  end

  def show_bytecode(line=@current_frame.line)
    meth = @current_frame.method
    start = meth.first_ip_on_line(line)
    fin = meth.first_ip_on_line(line+1)

    if fin == -1
      fin = meth.iseq.size
    end

    section "Bytecode between #{start} and #{fin-1} for line #{line}"

    partial = meth.iseq.decode_between(start, fin)

    ip = start

    partial.each do |ins|
      op = ins.shift

      ins.each_index do |i|
        case op.args[i]
        when :literal
          ins[i] = meth.literals[ins[i]].inspect
        when :local
          if meth.local_names
            ins[i] = meth.local_names[ins[i]]
          end
        end
      end

      info " %4d: #{op.opcode} #{ins.join(', ')}" % ip

      ip += (ins.size + 1)
    end
  end
end

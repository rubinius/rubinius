require 'readline'
require 'rubinius/debugger/frame'
require 'rubinius/debugger/commands'
require 'rubinius/debugger/breakpoint'
require 'rubinius/debugger/display'
require 'compiler/iseq'

#
# The Rubinius reference debugger.
#
# This debugger is wired into the debugging APIs provided by Rubinius.
# It serves as a simple, builtin debugger that others can use as
# an example for how to build a better debugger.
#

class Rubinius::Debugger
  include Rubinius::Debugger::Display

  # Find the source for the kernel.
  ROOT_DIR = File.expand_path("../", Rubinius::KERNEL_PATH)

  # Create a new debugger object. The debugger starts up a thread
  # which is where the command line interface executes from. Other
  # threads that you wish to debug are told that their debugging
  # thread is the debugger thread. This is how the debugger is handed
  # control of execution.
  #
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
      :show_ip              => false,
      :show_bytecode        => false,
      :highlight            => false,
      :list_command_history => {
        :path        => nil,
        :center_line => nil
      }
    }

    @loaded_hook = proc { |file|
      check_deferred_breakpoints
    }

    @added_hook = proc { |mod, name, exec|
      check_deferred_breakpoints
    }

    # Use a few Rubinius specific hooks to trigger checking
    # for deferred breakpoints.

    Rubinius::CodeLoader.loaded_hook.add @loaded_hook
    Rubinius.add_method_hook.add @added_hook

    @deferred_breakpoints = []

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

  # This is simplest API point. This starts up the debugger in the caller
  # of this method to begin debugging.
  #
  def self.here
    global.start(1)
  end

  # Startup the debugger, skipping back +offset+ frames. This lets you start
  # the debugger straight into callers method.
  #
  def start(offset=0)
    spinup_thread

    # Feed info to the debugger thread!
    locs = Rubinius::VM.backtrace(offset + 1, true)

    method = Rubinius::CompiledCode.of_sender

    bp = BreakPoint.new "<start>", method, 0, 0
    channel = Rubinius::Channel.new

    @local_channel.send Rubinius::Tuple[bp, Thread.current, channel, locs]

    # wait for the debugger to release us
    channel.receive

    Thread.current.set_debugger_thread @thread
    self
  end

  # Stop and wait for a debuggee thread to send us info about
  # stoping at a breakpoint.
  #
  def listen(step_into=false)
    while true
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

      if bp
        # Only break out if the hit was valid
        if bp.hit!(locs.first)
          if bp.has_condition?
            break if @current_frame.run(bp.condition)
          else
            break
          end
        end
      else
        break
      end
    end

    puts
    info "Breakpoint: #{@current_frame.describe}"
    show_code
    eval_code(@breakpoint.commands) if @breakpoint && @breakpoint.has_commands?

    if @variables[:show_bytecode]
      decode_one
    end

  end

  # Get a command from the user to run using readline
  #
  def accept_commands
    command_list_code = []
    cmd = Readline.readline "debug> "

    if cmd.nil?
      # ^D was entered
      cmd = "quit"
    elsif cmd.empty?
      cmd = @last_command
    else
      @last_command = cmd
    end

    command, args = cmd.to_s.strip.split(/\s+/, 2)

    runner = Command.commands.find { |k| k.match?(command) }

    if runner
      if runner == Command::CommandsList
        bp_id = (args || @breakpoints.size).to_i

        if @breakpoints.empty?
          puts "No breakpoint set"
          return
        elsif bp_id > @breakpoints.size || bp_id < 1
          puts "Invalid breakpoint number."
          return
        end

        puts "Type commands for breakpoint ##{bp_id}, one per line."
        puts "End with a line saying just 'END'."
        code = Readline.readline "> "
        while code != 'END'
          command_list_code << code
          code = Readline.readline "> "
        end
        args = {
          :bp_id => bp_id,
          :code  => command_list_code.empty? ? nil : command_list_code.join(";")
        }
      end

      runner.new(self).run args
    else
      puts "Unrecognized command: #{command}"
      return
    end

    # Save it to the history.
    @history_io.puts cmd
    unless command_list_code.empty?
      command_list_code << "END"
      @history_io.puts command_list_code.join("\n")
    end
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

  def set_breakpoint_method(descriptor, method, line=nil, condition=nil)
    exec = method.executable

    unless exec.kind_of?(Rubinius::CompiledCode)
      error "Unsupported method type: #{exec.class}"
      return
    end

    if line
      ip = exec.first_ip_on_line(line)

      if !ip
        error "Unknown line '#{line}' in method '#{method.name}'"
        return
      end
    else
      line = exec.first_line
      ip = 0
    end

    bp = BreakPoint.new(descriptor, exec, ip, line, condition)
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

  def add_deferred_breakpoint(klass_name, which, name, line)
    dbp = DeferredBreakPoint.new(self, @current_frame, klass_name, which, name,
                                line, @deferred_breakpoints)
    @deferred_breakpoints << dbp
    @breakpoints << dbp
  end

  def check_deferred_breakpoints
    @deferred_breakpoints.delete_if do |bp|
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

  def list_code_around_line(path, center_line, lines_to_show)
    lines_around = lines_to_show / 2
    start_line   = center_line - lines_around
    end_line     = center_line + lines_around

    list_code_range(path, start_line, end_line, center_line)
  end

  def list_code_range(path, start_line, end_line, center_line)
    if !File.exists?(path) && !File.exists?(File.join(@root_dir, path))
      error "Cannot find file #{path}"
      return
    end

    if start_line > @file_lines[path].size
      error "Line number #{@file_lines[path].size + 1} out of range: #{path} has #{@file_lines[path].size} lines."
      return
    end

    start_line = 1 if start_line < 1
    end_line   = @file_lines[path].size if end_line > @file_lines[path].size

    @variables[:list_command_history][:path]        = path
    @variables[:list_command_history][:center_line] = center_line

    (start_line).upto(end_line) do |i|
      show_code(i, path)
    end
  end

  def show_code(line = @current_frame.line,
                path = @current_frame.method.active_path)
    if str = @file_lines[path][line - 1]
      if @variables[:highlight]
        if fin = @current_frame.method.first_ip_on_line(line + 1)
          if name = send_between(@current_frame.method, @current_frame.ip, fin)
            str = str.gsub name.to_s, "\033[0;4m#{name}\033[0m"
          end
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
    decoder = Rubinius::InstructionDecoder.new(meth.iseq)
    partial = decoder.decode_between(ip, ip+1)

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

    if !fin
      fin = meth.iseq.size
    end

    section "Bytecode between #{start} and #{fin-1} for line #{line}"

    decoder = Rubinius::InstructionDecoder.new(meth.iseq)
    partial = decoder.decode_between(start, fin)

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

  private :spinup_thread

end

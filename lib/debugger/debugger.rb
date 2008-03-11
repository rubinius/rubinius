require 'debugger/command'
require 'debugger/output'
require 'thread'

# A debugger, providing a CLI for debugging Ruby code running under Rubinius.
# Takes advantage of Rubinius's support for full-speed debugging, via the
# +Breakpoint+ and +BreakpointTracker+ classes.
class Debugger
  @instance = nil
  @semaphore = Mutex.new

  # Only a single Debugger instance should be created
  def self.new
    super
  end
  self.private_class_method :new

  # Returns the singleton instance of the +Debugger+.
  def self.instance
    @semaphore.synchronize do
      @instance ||= new()
    end
  end

  def self.__instance__
    @instance
  end

  def self.__clear_instance
    @semaphore.synchronize do
      @instance = nil
    end
  end

  # Initializes a new +Debugger+ instance
  def initialize
    # HACK readline causes `rake spec` to hang in ioctl()
    require 'readline-native'

    @breakpoint_tracker = BreakpointTracker.new

    # Register this debugger as the default debug
    Rubinius::VM.debug_channel = @breakpoint_tracker.debug_channel

    @quit = false
    @breakpoint_listener = Thread.new do
      thrd = nil
      until @quit do
        @done = false
        begin
          thrd = @breakpoint_tracker.wait_for_breakpoint
          @breakpoint_tracker.wake_target(thrd) unless @quit  # defer wake until we cleanup
        rescue Error => e
          puts "An exception occured while processing a breakpoint:"
          puts e
        end
      end
      # Release singleton, since our loop thread is exiting
      Debugger.__clear_instance

      # Remove all remaining breakpoints
      @breakpoint_tracker.clear_breakpoints

      # De-register debugger on the global debug channel
      Rubinius::VM.debug_channel = nil

      if thrd
        # thrd will be nil if debugger was quit from other than a debug thread
        puts "[Debugger exiting]"
        @breakpoint_tracker.wake_target(thrd)
      end
      @breakpoint_tracker.release_waiting_threads
    end
    Thread.pass until waiting_for_breakpoint?
  end

  # Sets a breakpoint on a +CompiledMethod+ at the specified address.
  def set_breakpoint(cm, ip)
    @breakpoint_tracker.on(cm, :ip => ip) do |thread, ctxt, bp|
      activate_debugger thread, ctxt, bp
    end
  end

  # Removes the breakpoint(s) specified
  def remove_breakpoint(bp)
    if bp.kind_of? Breakpoint
      bp = [bp]
    end
    bp.each do |bp|
      @breakpoint_tracker.remove_breakpoint bp
    end
  end

  def step(selector)
    @breakpoint_tracker.step(selector) do |thread, ctxt, bp|
      activate_debugger thread, ctxt, bp
    end
  end

  # Clears all breakpoints
  def clear_breakpoints
    @breakpoint_tracker.clear_breakpoints
  end

  # Returns details of all breakpoints that are being managed by the debugger.
  # Note: This excludes transitory step breakpoints.
  def breakpoints
    @breakpoint_tracker.breakpoints
  end

  # Returns the breakpoint for the specified compiled method and IP
  def get_breakpoint(cm, ip)
    @breakpoint_tracker.get_breakpoint(cm, ip)
  end

  # True if the debugger is sleeping, i.e. waiting for a breakpoint to be hit.
  def waiting_for_breakpoint?
    @breakpoint_listener.status == 'sleep'
  end

  # Sets the done flag to true, so that the debugger resumes the debug thread.
  def done!
    @done = true
  end

  # Sets the quit flag to true, so that the debugger shuts down.
  def quit!
    @done = true
    @quit = true

    # If quit! is called from other than a command, we need to interrupt the
    # breakpoint listener thread
    unless @debug_thread
      @breakpoint_tracker.debug_channel.send nil
      @breakpoint_listener.join
    end
  end

  # (Re-)loads the available commands from all registered sub-classes of
  # Debugger::Command.
  def load_commands
    @commands = Debugger::Command.available_commands.map do |cmd_class|
      cmd_class.new
    end
    @commands.sort!
  end

  # Returns the available debugger commands, which are instances of all loaded
  # Debugger::Command subclasses.
  def commands
    load_commands unless @commands
    @commands
  end

  # Activates the debugger after a breakpoint has been hit, and responds to
  # debgging commands until a continue command is recevied.
  def activate_debugger(thread, ctxt, bp)
    puts "[Debugger activated]" unless bp.kind_of? StepBreakpoint
    @debug_thread = thread
    @debug_context = ctxt

    # Load debugger commands if we haven't already
    load_commands unless @commands

    file = @debug_context.file.to_s
    line = bp.line
    @prompt = "\nrbx:debug> "
    puts ""
    puts "#{file}:#{line} (#{@debug_context.method.name}) [IP:#{@debug_context.ip}]"
    lines = source_for(file)
    unless lines.nil?
      output = Output.new
      output.set_columns(['%d:', '%-s'])
      output.set_line_marker
      output.set_color :cyan
      output << [line, lines[line-1].chomp]
      output.set_color(:clear)
      puts output
    end

    until @done do
      inp = Readline.readline(@prompt)
      inp.strip!
      process_command(inp)
    end

    # Clear any references to the debuggee thread and context
    @debug_thread = nil
    @debug_context = nil
  end

  attr_accessor :prompt
  attr_reader :debug_thread, :debug_context

  # Processes a debugging command by finding a Command subclass that can handle
  # the input, and delegating to it.
  def process_command(inp)
    @commands.each do |cmd|
      if inp =~ cmd.command_regexp
        begin
          output = cmd.execute self, $~
          puts output if output
        rescue StandardError => e
          handle_exception e
        end
        break
      end
    end
  end

  # Handles any exceptions raised by a Command subclass during a debug session.
  def handle_exception(e)
    puts ""
    puts "An exception has occurred:\n    #{e.message} (#{e.class})"
    puts "Backtrace:"
    output = Output.new
    output.set_columns(['%s', '%-s'], ' at ')
    bt = e.awesome_backtrace
    first = true
    bt.frames.each do |fr|
      recv = fr[0]
      loc = fr[1]
      break if recv =~ /Debugger#process_command/
      output.set_color(bt.color_from_loc(loc, first))
      first = false # special handling for first line
      output << [recv, loc]
    end
    puts output
  end

  # Retrieves the source code for the specified file, if it exists
  def source_for(file)
    return @last_lines if file == @last_file
    @last_file, @last_lines = file, nil
    if File.exists?(file)
      @last_lines = File.readlines(file)
    end
  end
end



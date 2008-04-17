require 'debugger/command'
require 'debugger/interface'
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
    @breakpoint_tracker = BreakpointTracker.new do |thread, ctxt, bp|
      activate_debugger thread, ctxt, bp
    end

    # Register this debugger as the default debug channel listener
    Rubinius::VM.debug_channel = @breakpoint_tracker.debug_channel

    @quit = false
    @breakpoint_listener = Thread.new do
      thrd = nil
      until @quit do
        @done = false
        begin
          thrd = @breakpoint_tracker.wait_for_breakpoint
          @breakpoint_tracker.wake_target(thrd) unless @quit  # defer wake until we cleanup
        rescue Exception => e
          # An exception has occurred in the breakpoint or debugger code
          STDERR.puts "An exception occured while processing a breakpoint:"
          STDERR.puts e.to_s
          STDERR.puts e.awesome_backtrace
          # Attempt to resume blocked thread
          thrd.control_channel.send nil
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
        @breakpoint_tracker.wake_target(thrd)
      end
      @breakpoint_tracker.release_waiting_threads
    end
    Thread.pass until waiting_for_breakpoint?
  end

  # Sets a breakpoint on a +CompiledMethod+ at the specified address.
  def set_breakpoint(cm, ip)
    @breakpoint_tracker.on(cm, :ip => ip)
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

  # Temporarily removes a breakpoint, but does not delete it
  def disable_breakpoint(bp)
    @breakpoint_tracker.disable_breakpoint bp
  end

  # Temporarily removes a breakpoint, but does not delete it
  def enable_breakpoint(bp)
    @breakpoint_tracker.enable_breakpoint bp
  end

  def step(selector)
    @breakpoint_tracker.step(selector)
  end

  # Clears all breakpoints
  def clear_breakpoints
    @breakpoint_tracker.clear_breakpoints
  end

  # Returns details of all breakpoints that are being managed by the debugger.
  # Note: This excludes transitory step breakpoints.
  def breakpoints
    @breakpoint_tracker.global_breakpoints
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

  # Returns true if execution is about to be resumed
  def done?
    @done
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

  # Returns true if the debugger is shutting down
  def quit?
    @quit
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

  # Sets the interface to be used for the debugger
  def interface=(interface)
    @interface = interface
  end

  # Activates the debugger after a breakpoint has been hit, and responds to
  # debgging commands until a continue command is recevied.
  def activate_debugger(thread, ctxt, bp_list)
    @debug_thread = thread
    @eval_context = @debug_context = ctxt

    # Load debugger commands if we haven't already
    load_commands unless @commands
    # Default to command-line interface if none configured
    self.interface = CmdLineInterface.new unless @interface

    @interface.process_commands(self, thread, ctxt, bp_list)

    # Clear any references to the debuggee thread and context
    @debug_thread = nil
    @debug_context = nil
    @eval_context = nil
  end

  # The current thread and context being debugged
  attr_reader :debug_thread, :debug_context
  # The current eval context, i.e. the context in which most commands will be
  # executed. By default, this is the same as the debug context, but it can be
  # changed via the Up/Down commands.
  attr_accessor :eval_context

  # Flags to the debugger that a command requires more input
  def more_input!
    @more_input = true
  end

  def more_input?
    @more_input
  end

  # Processes a debugging command by finding a Command subclass that can handle
  # the input, and delegating to it.
  def process_command(inp)
    if @more_input and @last_command
      @more_input = false
      output = @last_command.execute(self, inp)
    else
      @more_input = false
      @commands.each do |cmd|
        if inp =~ cmd.command_regexp
          begin
            @last_command = cmd.multiline? ? cmd : nil
            output = cmd.execute(self, $~)
          rescue StandardError => e
            handle_exception e
          end
          break
        end
      end
    end
    output
  end

  # Handles any exceptions raised by a Command subclass during a debug session;
  # delegates to the debugger interface implementation.
  def handle_exception(e)
    @interface.handle_exception e
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



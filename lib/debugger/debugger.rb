require 'debugger/command'

# A debugger, providing a CLI for debugging Ruby code running under Rubinius.
# Takes advantage of Rubinius's support for full-speed debugging, via the
# +Breakpoint+ and +BreakpointTracker+ classes.
class Debugger
  @@instance = nil

  # Only a single Debugger instance should be created
  def self.new
    super
  end
  self.private_class_method :new

  # Returns the singleton instance of the +Debugger+.
  def self.instance
    @@instance ||= new()
  end

  # Initializes a new +Debugger+ instance
  def initialize
    require 'readline'

    @bt = BreakpointTracker.new

    # Register this debugger as the default debug
    Rubinius::VM.debug_channel = @bt.debug_channel

    @quit = false
    @breakpoint_listener = Thread.new do
      thrd = nil
      until @quit do
        @done = false
        thrd = @bt.process
        @bt.wake_target(thrd) unless @quit  # defer wake until we cleanup
      end
      # Release singleton, since our loop thread is exiting
      @@instance = nil

      # Remove all remaining breakpoints
      clear_breakpoints

      # De-register debugger on the to-be global debug channel
      Rubinius::VM.debug_channel = nil

      if thrd
        # thrd will be nil if debugger was quit from other than a debug thread
        puts "[Debugger exiting]"
        @bt.wake_target(thrd)
      end
    end
  end

  # Sets a breakpoint on a +CompiledMethod+ at the specified address.
  def set_breakpoint(cm, ip)
    @bt.on(cm, :ip => ip) do |thread, ctxt, handler|
      activate_debugger thread, ctxt
    end
  end

  # Removes the breakpoint(s) specified
  def remove_breakpoint(bp)
    if bp.kind_of? Breakpoint
      bp = [bp]
    end
    bp.each do |bp|
      @bt.remove bp
    end
  end

  # Clears all breakpoints
  def clear_breakpoints
    remove_breakpoint @bt.breakpoints
  end

  # Returns details of all breakpoints that are being managed by the debugger.
  def breakpoints
    @bt.breakpoints
  end

  # Returns the breakpoint for the specified compiled method and IP
  def get_breakpoint(cm, ip)
    @bt.get_breakpoint(cm, ip)
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
      @bt.debug_channel.send nil
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
  def activate_debugger(thread, ctxt)
    puts "[Debugger activated]"
    @debug_thread = thread
    @debug_context = ctxt

    # Load debugger commands if we haven't already
    load_commands unless @commands

    @prompt = "\nrbx:debug> "
    puts ""
    puts "#{@debug_context.file}:#{@debug_context.line} [IP:#{@debug_context.ip}]"
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
    puts e.backtrace.show
  end
end



require 'debugger/command'

# A debugger, providing a CLI for debugging Ruby code running under Rubinius.
# Takes advantage of Rubinius's support for full-speed debugging, via the
# +Breakpoint+ and +BreakpointTracker+ classes.
class Debugger
  @instance = nil

  # Only a single Debugger instance should be created
  def new
    super
  end
  private :new

  # Initializes a new +Debugger+ instance
  # TODO: Switch to a single global debug channel, and register this instance
  # when the Debugger is initialized.
  def initialize
    require 'readline'

    @bt = BreakpointTracker.new
    @commands = Debugger::Command.available_commands.map do |cmd_class|
      cmd_class.new
    end
    @commands.sort!

    @loop_thread = Thread.new do
      while true do
        @done = false
        @bt.process
      end

      # TODO: Remove all remaining breakpoints
      # TODO: De-register debugger on the to-be global debug channel
      puts "[Debugger exiting]"
    end

    # Register this debugger as the default debug
    Rubinius::VM.set_debug_channel @bt.debug_channel
  end

  attr_reader :commands

  # Returns the singleton instance of the +Debugger+.
  def self.instance
    @instance ||= Debugger.new
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
    @loop_thread.status == 'sleep'
  end

  # Sets the done flag to true, so that the debugger exits. Used by commands
  # that require the debugger to suspend and resume execution of the debuggee
  # thread.
  def done!
    @done = true
  end

  # Activates the debugger after a breakpoint has been hit, and responds to
  # debgging commands until a continue command is recevied.
  def activate_debugger(thread, ctxt)
    puts "[Debugger activated]"
    @debug_thread = thread
    @debug_context = ctxt

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



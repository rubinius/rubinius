# depends on: class.rb iseq.rb array.rb

##
# Breakpoint objects represent debugging breakpoints on CompiledMethods.
# In Rubinius, a breakpoint is represented by substituting the special opcode
# yield_debugger into the instruction sequence at the point at which execution
# is to be suspended. When the VM hits this opcode, it signals the debug thread
# that is waiting on the running thread's debug_channel, and then suspends the
# running thread until a signal is received on its control_channel.
#
# Breakpoint is a superclass to a number of subclasses used for different
# breakpoint scenarios. In general, a Breakpoint subclass is probably more
# appropriate than an instance of Breakpoint itself.

class Breakpoint

  ##
  # Creates a breakpoint in the supplied CompiledMethod object.
  #
  # The instruction pointer within the method that is to be the location of the
  # breakpoint is optional; if not specified, the breakpoint is inserted at the
  # start of the method.
  #
  # A block that is to be executed when the breakpoint is hit must be supplied.
  # It will be called with three parameters: the Thread that hit the
  # breakpoint, the MethodContext active when the breakpoint was hit, and this
  # Breakpoint instance.

  def initialize(cm, selector={}, &prc)
    unless block_given?
      raise ArgumentError, "A block must be supplied to be executed when the breakpoint is hit"
    end

    selector = {:ip => selector} if selector.kind_of? Fixnum
    if line = selector[:line]
      ip = cm.first_ip_on_line(line)
    elsif op = selector[:ip]
      ip = op
    end

    @method = cm
    @ip = ip || 0
    @handler = prc
    @enabled = false
    @encoder = InstructionSequence::Encoder.new

    validate_breakpoint_ip @ip
  end

  attr_reader :method
  attr_reader :ip

  # Verifies that the specified instruction pointer is a valid instruction
  # pointer address for the compiled method.
  def validate_breakpoint_ip(ip)
    bc = @method.bytecodes.decode
    i = 0
    bc.each do |op|
      return true if i == ip
      i += op.first.size
    end

    # If we get here, there's a problem with the IP
    max_ip = i
    max_ip -= bc.last.first.size if bc.last
    if ip < 0 or ip > max_ip
      raise ArgumentError, "Breakpoint (IP:#{ip}) is outside valid range of 0 to #{max_ip}"
    else
      # Ensure specified ip is at an instruction boundary
      raise ArgumentError, "Breakpoint (IP:#{ip}) is not aligned at an instruction boundary"
    end
  end

  # Returns the source file line that the breakpoint is on
  def line
    @method.line_from_ip(@ip)
  end

  def enabled?
    @enabled
  end

  ##
  # Makes the breakpoint active by inserting a yield_debugger instruction at the
  # breakpoint.

  def enable(ctxt=nil)
    unless @enabled
      @original_instruction = @encoder.replace_instruction(@method.bytecodes, @ip, [:yield_debugger])
      modify_iseq ctxt
      @enabled = true
    end
    @ip
  end

  ##
  # Makes the breakpoint inactive by removing the yield_debugger instruction at
  # the breakpoint. Returns the address of the breakpoint, so that the context
  # IP can be reset and execution can proceed following the breakpoint.

  def disable(ctxt=nil)
    if @enabled and @original_instruction.first != :yield_debugger
      @encoder.replace_instruction(@method.bytecodes, @ip, @original_instruction)
      modify_iseq ctxt
      @enabled = false
    end
    @ip
  end

  ##
  # Provide an accessor to set the original instruction in cases where a
  # breakpoint is set at a location that already has a breakpoint enabled.
  attr_accessor :original_instruction

  # Modifies an instruction sequence on either the compiled method (for a
  # global change), or a single context.
  def modify_iseq(ctxt)
    if ctxt
      bc = @method.bytecodes.dup
      ctxt.set_iseq(bc)
    else
      @method.compile
    end
  end
  protected :modify_iseq

  ##
  # Disables the breakpoint, and reloads the instruction sequence on the
  # supplied MethodContext object.

  def restore_into(ctx)
    ctx.ip = disable
  end

  def call_handler(thread, ctx)
    @handler.call(thread, ctx, self)
  end
end

##
# A breakpoint that is to be triggered once only in a certain number of steps
# (either lines or instructions) or at a specified IP address or line number.

class StepBreakpoint < Breakpoint
end


##
# Tracks and manages +Breakpoint+s
# Breakpoints fall into two distinct categories: those that are global in scope,
# effecting any thread that encounters them, and those that are specific to the
# execution context of a single task/thread.
# Global breakpoints are handled by modifying the compiled method instruction
# sequence that is used by all threads/contexts. Subsequently, only a single
# global breakpoint may be registered at any given instruction address.
# Context-specific breakpoints are handled by modifying the instruction sequence
# that is being used by a specific method or block context; these context-
# specific breakpoints are used for stepping, and for temporarily disabling (and
# subsequently re-enabling) a persistent global breakpoint that has been
# encountered by a given thread/context so that execution can be resumed.
# Context specific instruction sequence changes are transitory rather than
# persistent, and when the changes wrought to the context instruction sequence
# are no longer required, the global instruction sequence (complete with any
# persistent breakpoints) is switched back into the context.
# Context-specific breakpoints are layered on top of global breakpoints, and it
# is possible to have multiple context-specific breakpoint actions registered for
# the same instruction address, e.g. a restore breakpoint and a step breakpoint
# can both exist at the same location.
class BreakpointTracker

  def initialize
    # Global breakpoints are tracked by compiled method and IP
    @global_breakpoints = Hash.new {|h,k| h[k] = {}}
    # Context breakpoints are tracked by context and IP
    @context_breakpoints = Hash.new {|h,k| h[k] = []}
    @debug_channel = Channel.new
  end
  
  attr_accessor :debug_channel

  ##
  # Registers this breakpoint tracker as the debug handler for the specified
  # thread. The thread's existing control channel (if any) is not modified.
  #
  # Note: A debug channel must be established and registered before a breakpoint
  # is encountered during code execution. There are two ways in which this may
  # be done:
  # 1. Register a VM-wide debug channel (via Rubinius::VM.debug_channel=)
  #    that will be used for all threads that do not have a debug channel
  #    already set.
  # 2. Call Thread#set_debugging or Task#set_debugging to set a specific debug
  #    channel for the thread/task.
  #
  # This method uses the second option, and sets the BreakpointTracker's debug
  # channel as the debug channel for the specified thread. As an alternative to
  # calling this method, consider using Rubinius::VM.debug_channel= with the
  # BreakpointTracker's debug channel instead; this has the advantage of
  # handling breakpoints encountered by ANY thread without the need for pre-
  # registering a debug channel on any and every thread that might encounter a
  # breakpoint.

  def debug_thread(thr)
    thr.set_debugging @debug_channel, thr.control_channel
  end

  ##
  # Adds a breakpoint to the specified Method or CompiledMethod.
  # An options hash is used to specify various breakpoint settings:
  # - line: Set the breakpoint at the first instruction on or after the 
  # specified line number in the method source.
  # - ip: Set the breakpoint at the specified instruction pointer address.
  # - disabled: Create the breakpoint but do not enable it. This registers a
  # breakpoint, but does not insert the yield_debugger instruction in the
  # compiled method.
  def on(method, selector={}, &prc)
    cm = method
    if method.kind_of? Method or method.kind_of? UnboundMethod
      cm = method.compiled_method
    end

    bp = Breakpoint.new(cm, selector, &prc)
    if @global_breakpoints[cm][bp.ip]
      raise ArgumentError, "A breakpoint is already set for #{cm.name} at IP:#{bp.ip}"
    end

    @global_breakpoints[cm][bp.ip] = bp
    unless selector[:disabled]
      bp.enable
    end

    return bp
  end

  def remove_breakpoint(bp)
    bp.disable if bp.enabled?
    @global_breakpoints[bp.method].delete(bp.ip)
  end

  def get_breakpoint(cm, ip)
    @global_breakpoints[cm][ip]
  end

  ##
  # Locates the Breakpoint for the specified execution context

  def find_breakpoint(ctx)
    cm = ctx.method
    ip = ctx.ip - 1
    @global_breakpoints[cm][ip]
  end

  ##
  # Processes a breakpoint that has been triggered. A breakpoint listener thread
  # should call this method when it wants to wait for another thread to hit a
  # breakpoint.
  # Returns the thread object that hit the breakpoint, so that the caller can
  # wake the thread when finished with it.
  def wait_for_breakpoint
    if thread = @debug_channel.receive
puts "Breakpoint hit"
      ctx = thread.task.current_context

      # First, find any thread-specific breakpoints

      bp = find_breakpoint(ctx)
      unless bp
        thread.raise "Unable to find breakpoint for #{ctx.inspect}"
      end
      if bp.enabled?
        bp.restore_into(ctx)
      end

      begin
puts "Calling"
        bp.call_handler(thread, ctx)
      rescue Error => e
        puts "An exception occured in a breakpoint handler:"
        puts e
      end
    end
    thread
  end

  ##
  # Wakes the debuggee thread and continues execution until the next breakpoint

  def wake_target(thrd)
    thrd.control_channel.send nil
  end

  ##
  # Returns an array of +Breakpoint+ objects that correspond to the breakpoints
  # currently set.

  def breakpoints
    @global_breakpoints.values.map!{|h| h.values}.flatten
  end

  # Clears all breakpoints
  def clear_breakpoints
    breakpoints.each {|bp| remove_breakpoint bp}
    remove_breakpoint @step_breakpoint if @step_breakpoint
  end

  # Release any threads that have hit breakpoints, but not yet had their
  # handlers called.
  def release_waiting_threads
    while @debug_channel.waiting.count > 0
      thr = @debug_channel.receive
      thr.control_channel.send nil
    end
  end

end


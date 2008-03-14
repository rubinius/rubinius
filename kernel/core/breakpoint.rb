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
# breakpoint scenarios.
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
    ctx.ip = disable(ctx)
  end

  # Executes the callback block that was provided for when a breakpoint was hit.
  def call_handler(thread, ctx)
    @handler.call(thread, ctx, self)
  end
end


class TaskBreakpoint < Breakpoint
  # Initializes a new instance of a step breakpoint
  def initialize(task, &prc)
    unless block_given?
      raise ArgumentError, "A block must be supplied to be executed when the breakpoint is hit"
    end
    @task = task
    @handler = prc
    @encoder = InstructionSequence::Encoder.new
  end
  attr_reader :task
end


##
# A breakpoint that is to be triggered once only in a certain number of steps
# (either lines or instructions) or at a specified IP address or line number.
class StepBreakpoint < TaskBreakpoint

  ##
  # Initializes a step breakpoint. Takes the following parameters:
  # task: The task whose execution is to be stepped.
  # A selector hash specifying details of the type of step to be performed.
  # Valid keys are:
  #   step_type: A symbol specifying the type of stepping to perform; valid values
  #     are :target, :next, or :in. Defaults to :next.
  #   step_by: A symbol specifying whether to step by :line, :ip, or :caller
  #     (i.e. when stepping out). Defaults to :line.
  #   target: If the target is a specific line number or IP address, then target
  #     is that line number or IP address. Otherwise, it is nil.
  #   steps: The number of steps of the step_type to take. Ignored if a target is
  #     specified. Defaults to 1.
  # A block containing the action(s) to be performed when the step is complete.
  def initialize(task, selector, &prc)
    super(task, &prc)

    if selector[:line]
      # Shortcut selector to step to a line
      @step_type = :target
      @step_by = :line
      @target = selector[:line]
      @steps = nil
    elsif selector[:ip]
      # Shortcut selector to step to an IP address
      @step_type = :target
      @step_by = :ip
      @target = selector[:ip]
      @steps = nil
    elsif selector[:out]
      # Shortcut selector to step out
      @step_type = :target
      @step_by = :caller
      @target = nil
      @steps = selector[:out] || 1
    else
      @step_type = selector[:step_type] || :next
      @step_by = selector[:step_by] || :line
      @target = selector[:target]
      if @target
        @steps = nil
      else
        @steps = selector[:steps] || 1
        if @step_by == :line
          ctxt = task.current_context
          @last_file = ctxt.file
          @last_line = ctxt.line
        end
      end
    end
    raise ArgumentError, "Steps must be >= 1" if @steps and @steps < 1
  end

  attr_reader :context    # Context in which the next step breakpoint is to occur
  attr_reader :ip         # IP at which the next step breakpoint is to occur
  attr_reader :step_type
  attr_reader :step_by
  attr_reader :target
  attr_reader :steps
  attr_reader :break_type # The type of step break that is set

  # Determines where to set the next breakpoint for this step breakpoint, and
  # then sets it using the appropriate method. When we know where execution will
  # go, we set a yield_debugger at the appropriate point; when we don't (i.e.
  # because we are calling a method, or TODO: raising an exception, we set a
  # flag on the task so that the VM calls us when the context change is complete.
  def set_next_breakpoint
    # Record location of last step position
    @context = @task.current_context

    if @step_type == :target  # Stepping to a fixed location
      calculate_target_breakpoint
    else
      # Locate the IP at n steps past the current IP/line
      calculate_next_breakpoint
    end

    if @break_type == :opcode_replacement
      # Set new breakpoint
      # TODO: Need to ensure this handles multiple breakpoints being set at the same location
      @method = @context.method
      enable(@context)
    elsif @break_type == :context_change
      # Stepping into method, so set breakpoint on context change
      @task.debug_context_change = true
    end
  end

  # Calculates a target breakpoint, i.e. one where the final step destination
  # is knowable. When a step breakpoint is set with a target, we can just set
  # a yield_debugger at the specified target and then wait for it to be hit.
  def calculate_target_breakpoint
    @break_type = :opcode_replacement
    ctxt = @context
    mthd = ctxt.method
    ip = ctxt.ip
    bc = mthd.decode

    if @step_by == :line
      # Find IP of line to step to
      first_line = mthd.first_line
      last_line = mthd.line_from_ip(bc.last.ip)
      if @target >= first_line and @target <= last_line
        # within the current method/block or a contained method/block
        # TODO: could still be in contained block rather than current method...
        @ip = mthd.first_ip_on_line(@target)
      else
        # TODO: If target line is not in current method... step out? find it somehow?
        raise ArgumentError, "Target line is not within the current #{mthd.is_block? ? 'block' : 'method'}"
      end
    elsif @step_by == :ip
      # Stepping to specified IP
      i = bc.ip_to_index(@target)  # Will throw ArgumentError if IP outside valid range
      @ip = bc[i].ip
    elsif @step_by == :caller  # Stepping out 1 or more levels
      while @steps > 0
        ctxt = ctxt.sender if ctxt.sender
        @steps -= 1
      end
      raise ArgumentError, "Cannot step out of top-level context" if ctxt == @context
      @context = ctxt
      @ip = ctxt.ip
    else
      # Shouldn't get here
      raise ArgumentError, "Unsupported step_by type"
    end

    return @ip
  end

  # Calculates where to set the next breakpoint as we count down until our
  # step breakpoint is ready to fire. When stepping by an increment, it is
  # necessary to allow execution to proceed until the remaining steps are
  # reduced to 0. This is straightforward enough when stepping over code in a
  # single method with no branching (and can be calculated in a single pass),
  # but when the flow of execution is non-sequential, we need to break at the
  # points at which execution can fork, and determine where to set the next
  # breakpoint.
  def calculate_next_breakpoint
    raise "Should not be called when @steps == 0" if @steps == 0
    ctxt = @context
    ip = ctxt.ip
    mthd = ctxt.method
    bc = mthd.decode
    i = bc.ip_to_index(ip)

    # Determine if we have started a new line
    if @step_by == :line
      line = bc[i].line
      if @last_method and @last_ip
        @steps -= 1 if  @last_method.file != mthd.file or @last_method.line_from_ip(@last_ip) != line
      end
    end
    @last_method = mthd
    @last_ip = ip

    # Loop from the current instruction until a flow control change or steps
    # reduced to 0
    @ip = nil
    @break_type = nil
    bc[i..-1].each do |op|
      if @step_by == :line
        if op.line > line
          @steps -= 1 
          line = op.line
        end
      else
        # We've stepped if this instruction is not the starting instruction
        @steps -= 1 if op.ip > ip
      end

      flow = op.instruction.flow
      if @steps == 0 or (op.ip > ip and flow != :sequential)
        # At target, or else we need to stop stepping because we are at an 
        # opcode that may alter control flow. In the latter case, we will set
        # the next breakpoint once we reach the point where the flow change
        # occurs. This is so we do not set a breakpoint we might then hit before
        # we are supposed to; e.g. if a opcode causes the IP to be set to an
        # earlier value in the same method, it might be one we have not yet
        # executed, and so we could trigger the breakpoint too early if we set
        # it now.
        unless steps > 0 and flow == :send and @step_type != :in
          # There is one special case where we do nothing - when we are at a 
          # send, but we are not stepping in
          @ip = op.ip
        end
      elsif op.ip == ip and flow != :sequential
        # We are currently stopped on a flow changing opcode
        if flow == :send and @step_type == :in
          # We only need to stop at a send if we are stepping into called methods
          # Set flag so that we break after the target method is activated
          @ip = 0
          @break_type = :context_change
        elsif flow == :return
          # Switch to sender's context, and then find next breakpoint
          @context = @context.sender
          @ip = @context.ip
        elsif flow == :raise
          raise "Step handling on exception raise not yet implemented"
        elsif flow == :goto
          # At a flow control switch opcode; evaluate where execution will goto next
          case op.opcode
          when :goto
            @ip = op.args.first
          when :goto_if_true
            @ip = op.args.first if @task.get_stack_value(0)
          when :goto_if_false
            @ip = op.args.first unless @task.get_stack_value(0)
          when :goto_if_defined
            @ip = op.args.first unless @task.get_stack_value(0) == Undefined
          else
            raise "Unrecognized goto instruction"
          end
        end
        @steps -= 1 if @step_by == :ip and @ip
      end
      break if @ip
    end

    # Record last file/line we saw for next time
    @break_type = :opcode_replacement unless @break_type
    @ip
  end

  # Determines if the current execution context represented by ctxt is a
  # location where this breakpoint should fire.
  def trigger?(ctxt)
    if @break_type == :opcode_replacement
      ctxt == @context and ctxt.ip == (@ip + 1)
    elsif @break_type == :context_change
      ctxt != @context and ctxt.ip == 0
    end
  end

  def call_handler(thread, ctx)
    # Reset original opcode
    restore_into(ctx) if @break_type == :opcode_replacement

    if @target or @steps == 0
      # Final step destination reached
      @handler.call(thread, ctx, self)
    else
      # Calculate next stepping point on the way to step destination
      set_next_breakpoint
    end
  end
end


##
# Tracks and manages +Breakpoint+s.
#
# Breakpoints fall into two distinct categories: those that are global in scope,
# effecting any thread that encounters them, and those that are specific to the
# execution context of a single task/thread.
#
# Global breakpoints are handled by modifying the compiled method instruction
# sequence that is used by all threads/contexts. Subsequently, only a single
# global breakpoint may be registered at any given instruction address.
#
# Context-specific breakpoints are handled by modifying the instruction sequence
# that is being used by a specific method or block context; these context-
# specific breakpoints are used for stepping, and for temporarily disabling (and
# subsequently re-enabling) a persistent global breakpoint that has been
# encountered by a given thread/context so that execution can be resumed.
# Context specific instruction sequence changes are transitory rather than
# persistent, and when the changes wrought to the context instruction sequence
# are no longer required, the global instruction sequence (complete with any
# persistent breakpoints) is switched back into the context.
#
# Context-specific breakpoints are layered on top of global breakpoints, and it
# is possible to have multiple context-specific breakpoint actions registered for
# the same instruction address, e.g. a restore breakpoint and a step breakpoint
# can both exist at the same location.
class BreakpointTracker

  def initialize
    # Global breakpoints are tracked by compiled method and IP
    @global_breakpoints = Hash.new {|h,k| h[k] = {}}
    # Context breakpoints are tracked by task
    @task_breakpoints = Hash.new {|h,k| h[k] = []}
    @debug_channel = Channel.new
    @encoder = InstructionSequence::Encoder.new
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
    @global_breakpoints[cm][ip] if @global_breakpoints[cm]
  end

  ##
  # Sets a step breakpoint from the current debug task
  def step(selector, &prc)
    raise RuntimeError, "Can only step from a breakpoint" unless @thread
    task = @thread.task
    step_bp = StepBreakpoint.new(task, selector, &prc)
    step_bp.set_next_breakpoint
    @task_breakpoints[task] << step_bp
    step_bp
  end


  # Processes a breakpoint that has been triggered. A breakpoint listener thread
  # should call this method when it wants to wait for another thread to hit a
  # breakpoint.
  # Returns the thread object that hit the breakpoint, so that the caller can
  # wake the thread when finished with it.
  def wait_for_breakpoint
    if @thread = @debug_channel.receive
      task = @thread.task
      ctx = task.current_context
      bc = ctx.method.decode

      # First, find any task-specific breakpoints
      bp_list = []
      if task_bps = @task_breakpoints[task]
        task_bps.each do |bp|
          if bp.trigger?(ctx)
            bp_list << bp
            @task_breakpoints[task].delete(bp) if bp.steps == 0
          end
        end
      end

      # Next, check for global breakpoints
      bp = find_breakpoint(ctx)
      bp_list << bp if bp
      unless bp_list.size > 0
        thread.raise "Unable to find breakpoint for #{ctx.inspect}"
      end

      if bp and bp.enabled?
        bp.restore_into(ctx)
      end

      begin
        bp_list.each do |bp|
          bp.call_handler(@thread, ctx)
        end
      rescue RuntimeError => e
        puts "An exception occured in a breakpoint handler:"
        puts e
      end
    end
    @thread
  end

  ##
  # Wakes the debuggee thread and continues execution until the next breakpoint

  def wake_target(thrd)
    @thread = nil if thrd == @thread
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


module Rubinius
  ##
  # Breakpoint objects represent debugging breakpoints in the bytecode of
  # CompiledMethod objects.
  # --
  # In Rubinius, a breakpoint is represented by setting a flag on an opcode in
  # the instruction sequence at the point at which execution is to be suspended.
  # When the VM hits this opcode, it signals the debug thread that is waiting on
  # the running thread's debug_channel, and then suspends the running thread
  # until a signal is received on its control_channel.
  #
  # Breakpoint is a superclass to a number of subclasses used for different
  # breakpoint scenarios.

  class Breakpoint
    # This class is an abstract class; only subclasses ought to be instantiated.
    self.private_class_method :new

    # Define a single encoder instance to be used for all breakpoints
    # Returns the shared class InstructionSequence::Encoder instance.
    def Breakpoint.encoder
      @encoder ||= InstructionSequence::Encoder.new
    end

    # Initializes the breakpoint
    def initialize(&prc)
      @handler = prc
    end

    # Returns the Proc that is registered as the breakpoint callback
    attr_reader :handler
    # Returns the CompiledMethod on which the breakpoint is set
    attr_reader :method
    # Returns the IP address of the breakpoint
    attr_reader :ip

    # Returns the name of the file in which the breakpoint is set
    def file
      @method.file.to_s if @method
    end

    # Returns the source file line that the breakpoint is on, provided the
    # breakpoint is set at a particular location. Can return nil for
    # StepBreakpoint subclasses when a brekapoint is set on context change.
    def line
      @method.line_from_ip(@ip) if @ip
    end

    # Verifies that the specified instruction pointer is a valid instruction
    # pointer address for the compiled method.
    def validate_breakpoint_ip(ip)
      bc = @method.iseq.decode(false)
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

    ##
    # Makes the breakpoint active by setting a breakpoint flag on the instruction at
    # the breakpoint.
    def install(ctxt=nil, bytecodes=nil)
      @method.set_breakpoint @ip
    end

    ##
    # Removes the breakpoint by removing the yield_debugger flag at the
    # breakpoint. Returns the address of the breakpoint.
    def remove(ctxt=nil, bytecodes=@method.iseq)
      @method.clear_breakpoint @ip
      @ip
    end

    # Returns true if a breakpoint flag is currently set at the breakpoint target.
    def installed?
      if @method and @ip
        return @method.breakpoint? @ip
      end

      return false
    end

    # Executes the callback block that was provided for when a breakpoint was hit.
    def call_handler(thread, ctx)
      @handler.call(thread, ctx, self) if @handler
    end
  end


  # A GlobalBreakpoint represents a breakpoint that is installed on the master
  # copy of the bytecode of a CompiledMethod. This ensures it will be hit by any
  # thread that executes that method, unlike a StepBreakpoint which will only be
  # hit by a thread running a specific thread.
  class GlobalBreakpoint < Breakpoint
    self.public_class_method :new

    # Returns the next id to use for a breakpoint.
    def self.next_id
      @last_id = (@last_id || 0) + 1
    end

    ##
    # Creates a global breakpoint in the supplied CompiledMethod object.
    # Takes the following parameters:
    #   cm: The CompiledMethod in which the breakpoint is defined.
    #   selector: An optional hash specifying where in the CompiledMethod the
    #     breakpoint should be set. May be specified as either an instruction
    #     address using a key of :ip, or as a line number (:line).  The location
    #     for the breakpoint is optional; if not specified, the breakpoint is
    #     inserted at the start of the method.
    #   prc: An optional callback. A block that is to be executed when the
    #     breakpoint is hit may be supplied. It will be called with three
    #     parameters: the Thread that hit the breakpoint, the MethodContext active
    #     when the breakpoint was hit, and this Breakpoint instance.
    def initialize(cm, selector={}, &prc)
      super(&prc)

      selector = {:ip => selector} if selector.kind_of? Fixnum
      if line = selector[:line]
        ip = cm.first_ip_on_line(line)
      elsif op = selector[:ip]
        ip = op
      end

      @id = GlobalBreakpoint.next_id
      @method = cm
      @ip = ip || 0
      @enabled = selector[:disabled] ? selector[:disabled] : true
      self.condition = selector[:condition]
      @hits = 0
      validate_breakpoint_ip @ip
    end

    attr_reader :id

    def enable
      @enabled = true
    end

    def disable
      remove if installed?
      @enabled = false
    end

    def enabled?
      install
      @enabled
    end

    # Accessor for the number of breakpoint hits - i.e. the number of times the
    # breakpoint has fired.
    attr_reader :hits

    # Accessor for setting the condition on a breakpoint.
    # Conditions should be specified as a string to be eval-ed. A breakpoint
    # with a condition specified will only fire if the breakpoint condition
    # evaluates to true. Conditions can use the 'magic' variable \#hits to
    # specify a condition based upon the number of times the breakpoint has been
    # hit. The condition expression will be evaluated in the context of the
    # breakpoint location, so conditions can also evalaute expressions using
    # elements from the breakpoint context.
    # Attempts to set an invalid expression will raise an exception.
    def condition=(expr)
      @condition = expr
      if @condition
        # Convert the breakpoint expression to a valid snippet of Ruby
        eval_str = expr.gsub('#hits', '__hits__')
        # - check if then...end is needed
        if eval_str =~ /^(?:if|unless).+^(?:end)$/
          eval_str << " then true end"
        end

        # Test expression is valid syntax
        begin
          @condition_eval = eval_str if eval("return true; #{eval_str}")
        rescue Exception => e
          STDERR.puts "An error occurred while setting the breakpoint condition '#{condition}':"
          STDERR.puts e.to_s
          STDERR.puts "Breakpoint condition has been ignored."
          @condition = nil
          @condition_eval = nil
        end
      else
        @condition_eval = nil
      end
    end

    # Returns the condition (if any) that the breakpoint needs to satisfy in
    # order to be triggered.
    attr_reader :condition

    # Determines if the breakpoint should actually fire when hit.
    # This method evaluates any condition specified for the breakpoint. The
    # condition is evaluated with a binding from the execution context at the
    # point at which the breakpoint is set.
    def trigger?(ctx)
      @hits += 1
      return true unless @condition_eval
      begin
        # Evaluate the condition in the debug context
        proc = eval("Proc.new {|__hits__| #{@condition_eval} }",
                 Binding.setup(thread.current_context))
        proc.call(hits)
      rescue Exception => e
        # An exception occurred while processing the breakpoint condition
        STDERR.puts "An error occurred while processing the breakpoint condition '#{condition}':"
        STDERR.puts e.to_s
        false
      end
    end
  end


  ##
  # A breakpoint that is to be triggered once only in a certain number of steps
  # (either lines or instructions) or at a specified IP address or line number.
  class StepBreakpoint < Breakpoint
    self.public_class_method :new

    ##
    # Initializes a step breakpoint. Takes the following parameters:
    # thread: The thread whose execution is to be stepped.
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
    def initialize(thread, selector, &prc)
      super(&prc)

      @thread = thread

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
          @step_type = :target
          @steps = nil unless @step_by == :caller
        else
          @steps = selector[:steps] || 1
          if @step_by == :line
            ctxt = thread.current_context
            @last_file = ctxt.file
            @last_line = ctxt.line
          end
        end
      end
      raise ArgumentError, "Steps must be >= 1" if @steps and @steps < 1
    end

    attr_reader :thread
    attr_reader :context    # Context in which the next step breakpoint is to occur
    attr_reader :step_type
    attr_reader :step_by
    attr_reader :target
    attr_reader :steps
    attr_reader :break_type # The type of step break that is set

    # Determines where to set the next breakpoint for this step breakpoint, and
    # then sets it using the appropriate method. When we know where execution will
    # go, we set a yield_debugger at the appropriate point; when we don't (i.e.
    # because we are calling a method, or TODO: raising an exception), we set a
    # flag on the thread so that the VM calls us when the context change is complete.
    def set_next_breakpoint(bc=nil)
      # Record location of last step position
      @context = @thread.current_context

      if @step_type == :target  # Stepping to a fixed location
        calculate_target_breakpoint
      else
        # Locate the IP at n steps past the current IP/line
        calculate_next_breakpoint bc
      end

      if @break_type == :opcode_flag
        # Set new breakpoint
        @method = @context.method
      end
    end

    # Installs a step breakpoint into the specified thread.
    #
    # Unlike a global breakpoint, this requires the use of a context-specific copy
    # of the bytecode for the CompiledMethod where the next breakpoint is to set.
    # Note that this may be in a different CompiledMethod than the current
    # context, e.g. when the next breakpoint is in a caller because we are about
    # to return. If we are about to step into a new context, we will not have a
    # context yet on which the breakpoint can be set; in this case, we use a thread
    # flag to tell the VM to yield after creating the new context.
    def install(bytecodes=nil)
      if @break_type == :opcode_flag
        # Set new breakpoint
        super(@context, bytecodes)
      elsif @break_type == :context_change
        # Stepping into method, so set breakpoint on context change
        @thread.debug_context_change = true
      else
        super()
      end
    end

    ##
    # Makes the breakpoint inactive by removing the yield_debugger instruction at
    # the breakpoint. Returns the address of the breakpoint, so that the context
    # IP can be reset and execution can proceed following the breakpoint.
    def remove(ctxt)
      if @break_type == :context_change
        @thread.debug_context_change = false
      else
        super(ctxt)
      end
      @ip
    end

    # Calculates a target breakpoint, i.e. one where the final step destination is
    # knowable. When a step breakpoint is set with a target, we can just set a
    # yield_debugger at the specified target and then wait for it to be hit.
    def calculate_target_breakpoint
      @break_type = :opcode_flag
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
    def calculate_next_breakpoint(bc=nil)
      raise "Should not be called when @steps == 0" if @steps == 0

      ctxt = @context
      ip = ctxt.ip
      mthd = ctxt.method
      asm = bc ? mthd.decode(bc) : mthd.decode
      i = asm.ip_to_index(ip)

      # Determine if we have started a new line
      if @step_by == :line
        line = asm[i].line
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
      asm[i..-1].each do |op|
        if @step_by == :line
          if op.line > line
            @steps -= 1
            line = op.line
          end
        else
          # We've stepped if this instruction is not the starting instruction
          @steps -= 1 if op.ip > ip
        end
        flow = op.instruction.control_flow
        if @steps == 0 or (op.ip > ip and flow != :next)
          # At target, or else we need to stop stepping because we are at an
          # opcode that may alter control flow. In the latter case, we will set
          # the next breakpoint once we reach the point where the flow change
          # occurs. This is so we do not set a breakpoint we might then hit before
          # we are supposed to; e.g. if an opcode causes the IP to be set to an
          # earlier value in the same method, it might be one we have not yet
          # executed, and so we could trigger the breakpoint too early if we set
          # it now.
          unless steps > 0 and flow == :send and @step_type != :in
            # There is one special case where we do nothing - when we are at a
            # send, but we are not stepping in
            @ip = op.ip
          end
        elsif op.ip == ip and flow != :next
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
          elsif flow == :branch
            # At a flow control switch opcode; evaluate where execution will goto next
            case op.opcode
            when :goto
              @ip = op.args.first
            when :goto_if_true
              @ip = op.args.first if @thread.get_stack_value(0)
            when :goto_if_false
              @ip = op.args.first unless @thread.get_stack_value(0)
            else
              raise "Unrecognized goto instruction"
            end
          end
          @steps -= 1 if @step_by == :ip and @ip
        end
        break if @ip
      end

      # Record last file/line we saw for next time
      @break_type = :opcode_flag unless @break_type
      @ip
    end

    # Determines if the current execution context of the specified thread is a
    # location where this breakpoint should fire.
    def trigger?(thread)
      ctxt = thread.current_context
      mthd = ctxt.method
      ip = ctxt.ip
      if @break_type == :opcode_flag
        mthd == @method and ip == @ip
      elsif @break_type == :context_change
        mthd != @method and ip == 0
      end
    end

    # Calls the handler block registered for callbacks when a breakpoint is hit.
    # For StepBreakpoints, the handler is only called when the target is reached
    # or the steps counter reaches 0.
    def call_handler(thread, ctx)
      if @target or @steps == 0
        # Final step destination reached
        super(thread, ctx)
      end
    end
  end


  ##
  # Tracks and manages +Breakpoint+s.
  #
  # Breakpoints fall into two distinct categories: those that are global in scope,
  # effecting any thread that encounters them, and those that are specific to the
  # execution context of a single thread.
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

    def initialize(&callback)
      # Global breakpoints are tracked by compiled method and IP
      @global_breakpoints = Hash.new {|h,k| h[k] = {}}
      # Context breakpoints are tracked by thread
      @thread_breakpoints = Hash.new {|h,k| h[k] = []}
      @debug_channel = Rubinius::Channel.new
      @callback = callback
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
    # 2. Call Thread#set_debugging or Rubinius::Task#set_debugging to set a
    #    specific debug channel for the thread/thread.
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
    # Adds a persistent, global breakpoint to the specified Method or
    # CompiledMethod.
    #
    # An options hash is used to specify various breakpoint settings:
    # - line: Set the breakpoint at the first instruction on or after the
    #   specified line number in the method source.
    # - ip: Set the breakpoint at the specified instruction pointer address.
    # - disabled: Create the breakpoint but do not enable it. This registers a
    #   breakpoint, but does not insert the yield_debugger instruction in the
    #   compiled method.
    # - condition: An optional string representing an expression to be eval-ed
    #   each time the breakpoint is hit, to determine if it should fire.
    def on(method, selector={})
      cm = method
      if method.kind_of? Method or method.kind_of? UnboundMethod
        cm = method.executable
        unless cm.kind_of? CompiledMethod
          raise ArgumentError, "Method object wasn't bound to a CompiledMethod"
        end
      end

      selector = {:ip => selector} if selector.kind_of? Fixnum
      bp = GlobalBreakpoint.new(cm, selector)
      if @global_breakpoints[cm][bp.ip]
        raise ArgumentError, "A breakpoint is already set for #{cm.name} at IP:#{bp.ip}"
      end

      @global_breakpoints[cm][bp.ip] = bp
      unless selector[:disabled]
        bp.install
      end

      return bp
    end

    # Removes a global breakpoint, making sure the original instruction is restored
    def remove_breakpoint(bp)
      if bp.kind_of? Fixnum
        # Find the breakpoint with the specified id
        bp = get_breakpoint_by_id(bp)
      end
      if bp
        bp.remove
        @global_breakpoints[bp.method].delete(bp.ip)
        @global_breakpoints.delete(bp.method) if @global_breakpoints[bp.method].size == 0
      end
      bp
    end

    ##
    # Returns the global breakpoint at the specified location
    def get_breakpoint(cm, ip)
      @global_breakpoints[cm][ip] if @global_breakpoints[cm]
    end

    ##
    # Returns the global breakpoints for the specified CompiledMethod.
    # If no global breakpoints are set on the CM, returns nil.
    def get_breakpoints_on(cm)
      bp_list = @global_breakpoints[cm]
      bp_list &&= bp_list.values
    end

    # Finds a global breakpoint by it's id
    def get_breakpoint_by_id(id)
      global_breakpoints.each do |bp|
        return bp if bp.id == id
      end
      nil
    end

    ##
    # Sets a step breakpoint from the current debug thread.
    # +selector+ is a hash that specifies the step settings.
    def step(selector)
      raise RuntimeError, "Can only step from a breakpoint" unless @thread
      step_bp = StepBreakpoint.new(@thread, selector)
      @thread_breakpoints[@thread] << step_bp
      step_bp
    end

    # Finds all the breakpoints that are set on the specified thread and compiled
    # method, and which should be triggered.
    def find_breakpoints(thread, cm, ip)
      # First, find any thread-specific breakpoints
      bp_list = []

      if thread
        if thread_bps = @thread_breakpoints[thread]
          thread_bps.each do |bp|
            bp_list << bp if bp.trigger?(thread)
          end
        end
      end

      # Next, check for global breakpoints
      if cm
        bp = @global_breakpoints[cm][ip] if @global_breakpoints[cm]
        bp_list << bp if bp && bp.enabled?
      end
      bp_list
    end

    # Processes a breakpoint that has been triggered. A breakpoint listener thread
    # should call this method when it wants to wait for another thread to hit a
    # breakpoint.
    # Returns the thread object that hit the breakpoint, so that the caller can
    # wake the thread when finished with it.
    def wait_for_breakpoint
      if @thread = @debug_channel.receive
        ctx = @thread.context
        cm = ctx.method
        ip = ctx.ip

        @bp_list = find_breakpoints(@thread, cm, ip)
        unless @bp_list.size > 0
          raise "Unable to find any managed breakpoint for #{@thread.inspect} at IP:#{ctx.ip}"
        end

        do_yield = false
        @bp_list.each do |bp|
          case bp
          when StepBreakpoint
            if bp.steps.nil? or bp.steps == 0 then
              # Delete expired thread breakpoints
              @thread_breakpoints[@thread].delete(bp)
              do_yield = true
            end
          when GlobalBreakpoint
            do_yield = true if bp.trigger?(ctx)
          end

          # Call any breakpoint specific handler
          bp.call_handler(@thread, ctx)
        end

        begin
          @callback.call(@thread, ctx, @bp_list)
        rescue Exception => e
          STDERR.puts "An exception occurred in a breakpoint handler:"
          STDERR.puts e.to_s
          STDERR.puts e.awesome_backtrace
        end
      end
      @thread
    end

    ##
    # Wakes the debuggee thread and continues execution until the next breakpoint.
    def wake_target(thread)
      if thread == @thread
        # Activate any thread breakpoints; since these modify the global method
        # bytecode, it is easiest to only set the thread breakpoints once any
        # changes to global breakpoints are finalised.
        ctx = thread.context
        mthd = ctx.method
        bc = mthd.iseq.dup

        # Define a hash of thread-specific bytecodes for use when installing
        ctxt_bc = Hash.new {|h,mthd| h[mthd] = mthd ? mthd.iseq.dup : nil}
        ctxt_bc[mthd] = bc
        @thread_breakpoints[thread].each do |bp|
          if bp.trigger?(thread) or bp.break_type.nil?
            # Locate next step breakpoint location
            begin
              bp.set_next_breakpoint bc
            rescue => e
              puts "Error setting breakpoint: #{e.message}"
              bp = nil
            end
          end
          # Install the yield_debugger / debug_on_ctxt_change
          bp.install ctxt_bc[bp.method] if bp
        end

        @thread = nil
      end
      thread.control_channel.send nil
    end

    ##
    # Returns an array of +Breakpoint+ objects that correspond to the global
    # breakpoints currently set.
    def global_breakpoints
      @global_breakpoints.values.map!{|h| h.values}.flatten
    end

    # Clears all breakpoints, both global and thread; once this has been called,
    # code should execute without hitting any breakpoints.
    def clear_breakpoints
      global_breakpoints.each do |bp|
        bp.remove if bp.installed?
      end
      @thread_breakpoints.values.flatten do |bp|
        if bp.step_type == :opcode_flag
          bp.context.reload_method
        else
          bp.remove nil
        end
      end
      @global_breakpoints.clear
      @thread_breakpoints.clear
    end

    # Release any threads that have hit breakpoints, but not yet had their
    # handlers called.
    def release_waiting_threads
      while thr = @debug_channel.try_receive
        thr = @debug_channel.receive
        thr.control_channel.send nil
      end
    end

  end
end


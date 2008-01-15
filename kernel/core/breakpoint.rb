# depends on: class.rb iseq.rb array.rb

# +Breakpoint+ objects represent debugging breakpoints on +CompiledMethod+s.
# In Rubinius, a breakpoint is represented by substituting the special opcode
# yield_debugger into the instruction sequence at the point at which execution
# is to be suspended. When the VM hits this opcode, it signals the debug thread
# that is waiting on the running thread's debug_channel, and then suspends the
# running thread until a signal is received on its control_channel.
class Breakpoint

  # Creates a breakpoint in the supplied +CompiledMethod+ object.
  # The instruction pointer within the method that is to be the location of the
  # breakpoint is optional; if not specified, the breakpoint is inserted at the
  # start of the method.
  # A block that is to be executed when the breakpoint is hit must be supplied.
  # It will be called with three parameters: the +Thread+ that hit the
  # breakpoint, the +MethodContext+ active when the breakpoint was hit, and this
  # +Breakpoint+ instance.
  def initialize(cm, ip = nil, &prc)
    unless block_given?
      raise ArgumentError, "A block must be supplied to be executed when the breakpoint is hit"
    end

    @method = cm
    @ip = ip || 0
    @handler = prc
    @enabled = false
    @encoder = InstructionSequence::Encoder.new
    @hits = 0

    bc = @method.bytecodes.decode
    i = 0
    bc.each do |op|
      return if i == @ip
      i += op.first.size
    end

    # If we get here, there's a problem with the IP
    max_ip = i
    max_ip -= bc.last.first.size if bc.last
    if @ip < 0 or @ip > max_ip
      raise ArgumentError, "Breakpoint (IP:#{@ip}) is outside valid range of 0 to #{max_ip}"
    else
      # Ensure specified ip is at an instruction boundary
      raise ArgumentError, "Breakpoint (IP:#{@ip}) is not aligned at an instruction boundary"
    end
  end

  attr_reader :hits
  attr_reader :method
  attr_reader :ip

  def line
    @method.line_from_ip(@ip)
  end

  def enabled?
    @enabled
  end

  # Makes the breakpoint active by inserting a yield_debugger instruction at the
  # breakpoint.
  def enable
    unless @enabled
      @orig = @encoder.replace_instruction(@method.bytecodes, @ip, [:yield_debugger])
      @method.compile
      @enabled = true
    end
    @ip
  end

  # Makes the breakpoint inactive by removing the yield_debugger instruction at
  # the breakpoint. Returns the address of the breakpoint, so that the context
  # IP can be reset and execution can proceed following the breakpoint.
  def disable
    if @enabled
      @encoder.replace_instruction(@method.bytecodes, @ip, @orig)
      @method.compile
      @orig = nil
      @enabled = false
    end
    @ip
  end

  # Disables the breakpoint, and reloads the instruction sequence on the
  # supplied +MethodContext+ object.
  def restore_into(ctx)
    ctx.ip = disable
    ctx.reload_method
  end

  def call_handler(thread, ctx)
    @hits += 1
    @handler.call(thread, ctx, self)
  end
end


# Tracks and manages +Breakpoint+s
class BreakpointTracker

  def initialize
    @breakpoints = Hash.new { |h,k| h[k] = {} }
    @debug_channel = Channel.new
  end
  
  attr_accessor :debug_channel

  # Registers this breakpoint tracker as the debug handler for the specified
  # thread. The thread's existing control channel (if any) is not modified.
  def debug_thread(thr)
    thr.set_debugging @debug_channel, thr.control_channel
  end

  # Adds a breakpoint
  def on(method, opts, &prc)
    cm = method
    if method.kind_of? Method
      cm = method.compiled_method
    end

    if line = opts[:line]
      ip = cm.first_ip_on_line(line)
    elsif op = opts[:ip]
      ip = op
    else
      raise ArgumentError, "Unknown selector: #{opts.inspect}"
    end

    if @breakpoints[cm][ip]
      raise RuntimeError, "A breakpoint is already set for #{cm.name} at IP:#{ip}"
    end
    bp = Breakpoint.new(cm, ip, &prc)
    @breakpoints[cm][ip] = bp
    unless opts[:noinstall]
      bp.enable
    end
    return bp
  end

  def remove(bp)
    bp.disable if bp.enabled?
    @breakpoints[bp.method].delete(bp.ip)
  end

  def get_breakpoint(cm, ip)
    @breakpoints[cm][ip]
  end

  # Locates the +Breakpoint+ for the specified execution context
  def find_breakpoint(ctx)
    cm = ctx.method
    ip = ctx.ip - 1
    @breakpoints[cm][ip]
  end

  # Processes a breakpoint that has been triggered.
  def process
    thread = wait_for_breakpoint
    ctx = thread.task.current_context
    bp = find_breakpoint(ctx)
    unless bp
      raise "Unable to find breakpoint for #{ctx.inspect}"
    end
    if bp.enabled?
      bp.restore_into(ctx)
    end

    begin
      bp.call_handler(thread, ctx)
    rescue Error => e
      puts "An exception occured in a breakpoint handler:"
      puts e
    end
    wake_target thread
  end

  # Suspends the current thread until a breakpoint is signaled
  def wait_for_breakpoint
    @debug_channel.receive
  end
  
  # Wakes the debuggee thread and continues execution until the next breakpoint
  def wake_target(thrd)
    thrd.control_channel.send nil
  end

  # Returns an array of +Breakpoint+ objects that correspond to the breakpoints
  # currently set.
  def breakpoints
    @breakpoints.values.map!{|h| h.values}.flatten
  end
end

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
  # It will be called with two parameters: the +MethodContext+ active when the
  # breakpoint was hit, and this +Breakpoint+ instance.
  def initialize(cm, ip = nil, &prc)
    @method = cm
    @ip = ip || 0
    @handler = prc
    @enabled = false
    @encoder = InstructionSequence::Encoder.new

    bc = @method.bytecodes.decode
    i = 0
    bc.each do |op|
      return if i == @ip
      i += op.first.size
    end

    # If we get here, there's a problem with the IP
    max_ip = bc.size / InstructionSet::InstructionSize
    if @ip < 0 or @ip > max_ip
      raise ArgumentError, "Breakpoint (IP:#{@ip}) is outside valid range of 0 to #{max_ip}"
    else
      # Ensure specified ip is at an instruction boundary
      raise ArgumentError, "Breakpoint (IP:#{@ip}) is not aligned at an instruction boundary"
    end
  end

  attr_reader :method
  attr_reader :ip

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

  # Disables the breakpoint, and resets the supplied +MethodContext+ object to 
  def restore_into(ctx)
    ctx.ip = disable
    ctx.reload_method
  end

  def call_handler(ctx)
    @handler.call(ctx, self)
  end
end


# Tracks and manages +Breakpoint+s
class BreakpointTracker

  def initialize
    @breakpoints = Hash.new { |h,k| h[k] = {} }
    @debug_channel = Channel.new
    @control_channel = Channel.new
  end
  
  attr_accessor :debug_channel
  attr_accessor :control_channel

  # Registers this breakpoint tracker as the debug handler for the specified
  # thread. Note that all threads that might hit a breakpoint must have been
  # registered, or an error is raised by the VM.
  def debug_thread(thr)
    thr.set_debugging @debug_channel, @control_channel
  end

  # Adds a breakpoint
  def on(method, opts, &prc)
    if method.kind_of? Method
      cm = method.compiled_method
    else
      cm = method
    end

    if line = opts[:line]
      ip = cm.first_ip_on_line(line)
    elsif op = opts[:ip]
      ip = op
    else
      raise ArgumentError, "Unknown selector: #{opts.inspect}"
    end

    if @breakpoints[cm][ip]
      raise RuntimeError, "A breakpoint is already set #{@method.name} at IP:#{@ip}"
    end
    
    bp = Breakpoint.new(cm, ip, prc)
    @breakpoints[cm][ip] = bp
    unless opts[:noinstall]
      bp.enable
    end
    return bp
  end

  # Locates the +Breakpoint+ for the specified execution context
  def find_handler(ctx)
    cm = ctx.method
    ip = ctx.ip - 1
    @breakpoints[cm][ip]
  end
  
  def process
    ctx = wait_for_breakpoint
    pnt = find_handler(ctx)
    unless pnt
      raise "Unable to find handler for #{ctx.inspect}"
    end
    if pnt.installed?
      pnt.restore_into(ctx)
    end
    pnt.call(ctx)
    wake_target
  end

  def wait_for_breakpoint
    @debug_channel.receive
  end
  
  def wake_target
    @control_channel.send nil
  end

  # Returns an array of +Breakpoint+ objects that correspond to the breakpoints
  # currently set.
  def breakpoints
    @breakpoints.values.map!{|h| h.values.first}
  end
end

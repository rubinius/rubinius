# depends on: module.rb kernel.rb

module Type
  # The order seems backwards and it is. But the Type.coerce_to compiler
  # plugin is able to send this order much easier than in reverse, so
  # we use this.
  def self.coerce_failed(cls, obj)
    raise TypeError, "A #{obj.class} tried to become a #{cls} but failed"
  end
  
  def self.coerce_unable(exc, cls, obj)
    raise TypeError, "Unable to convert a #{obj.class} into a #{cls}"
  end

  # By default, the inline version of Type.coerce_to is used by the compiler.
  # This is here for completeness but is rarely called. Changes to it
  # are not reflected in existing calles to Type.coerce_to without changes
  # to the compiler.
  def self.coerce_to(obj, cls, meth)
    return obj if obj.kind_of?(cls)
    begin
      ret = obj.__send__(meth)
    rescue Object => e
      coerce_unable(e, cls, obj)  
    end
    
    if ret.kind_of?(cls)
      return ret
    else
      coerce_failed(cls, obj)
    end
  end
end

module Kernel
  def Float(obj)
    raise TypeError, "can't convert nil into Float" if obj.nil?
    
    if obj.is_a?(String)
      if obj !~ /^(\+|\-)?\d+$/ && obj !~ /^(\+|\-)?(\d_?)*\.(\d_?)+$/ && obj !~ /^[-+]?\d*\.?\d*e[-+]\d*\.?\d*/
        raise ArgumentError, "invalid value for Float(): #{obj.inspect}"
      end
    end
  
    Type.coerce_to(obj, Float, :to_f)
  end
  
  def Integer(obj)
    return obj.to_inum(0, true) if obj.is_a?(String)
    method = obj.respond_to?(:to_int) ? :to_int : :to_i
    Type.coerce_to(obj, Integer, method)
  end

  def Array(obj)
    if obj.respond_to?(:to_ary)
      Type.coerce_to(obj, Array, :to_ary)
    elsif obj.respond_to?(:to_a)
      Type.coerce_to(obj, Array, :to_a)
    else
      [obj]
    end
  end

  def String(obj)
    Type.coerce_to(obj, String, :to_s)
  end
  
  # MRI uses a macro named StringValue which has essentially
  # the same semantics as obj.coerce_to(String, :to_str), but
  # rather than using that long construction everywhere, we
  # define a private method similar to String(). Another
  # possibility would be to change String() as follows:
  #   String(obj, sym=:to_s)
  # and use String(obj, :to_str) instead of StringValue(obj)
  def StringValue(obj)
    Type.coerce_to(obj, String, :to_str)
  end
  private :StringValue
  
  # Reporting methods
  
  # HACK :: added due to broken constant lookup rules
  def raise(exc=$!, msg=nil, trace=nil)
    if exc.respond_to? :exception
      exc = exc.exception msg
      raise ::TypeError, 'exception class/object expected' unless exc.kind_of?(::Exception)
      exc.set_backtrace trace if trace
    elsif exc.kind_of? String or !exc
      exc = ::RuntimeError.exception exc
    else
      raise ::TypeError, 'exception class/object expected'
    end

    if $DEBUG and $VERBOSE != nil
      STDERR.puts "Exception: #{exc.message} (#{exc.class})"
    end

    exc.set_backtrace MethodContext.current.sender unless exc.backtrace
    Rubinius.asm(exc) { |e| e.bytecode(self); raise_exc }
  end
  
  alias_method :fail, :raise
  
  def warn(warning)
    unless $VERBOSE.nil?
      $stderr.write warning
      $stderr.write "\n"
    end
    nil
  end

  def exit(code=0)
    code = 0 if code.equal? true
    raise SystemExit.new(code)
  end
  
  private :exit

  def exit!(code=0)
    Process.exit(code)
  end
  
  private :exit!

  def abort(msg=nil)
    $stderr.puts(msg) if(msg)
    exit 1
  end  
  
  # Display methods
  def printf(*args)
    if args[0].class == IO
      args[0].write(Sprintf::Parser.format(args[1], args[2..-1]))
    elsif args[0].class == String
      $stdout.write(Sprintf::Parser.format(args[0], args[1..-1]))
    else
      raise TypeError, "The first arg to printf should be an IO or a String"
    end
    nil
  end

  def sprintf(str, *args)
    Sprintf::Parser.format(str, args)
  end
  alias_method :format, :sprintf
  
  def puts(*a)
    $stdout.puts(*a)
    return nil
  end
  
  def p(*a)
    a = [nil] if a.empty?
    a.each { |obj| $CONSOLE.puts obj.inspect }
    nil
  end

  def print(*args)
    args.each do |obj|
      $CONSOLE.write obj.to_s
    end
    nil
  end
    
  def open(*a, &block)
    if !a.empty?
      first = a.first
      if first.kind_of? String and a.first.prefix? '|'
        a[0] = a.first[1..-1]
        return IO.popen(*a, &block)
      end
    end
      
    File.open(*a, &block)
  end

  # NOTE - this isn't quite MRI compatible.
  # we don't seed the RNG by default with a combination
  # of time, pid and sequence number
  def srand(seed)
    cur = Kernel.current_srand
    Platform::POSIX.srand(seed.to_i)
    Kernel.current_srand = seed.to_i
    return cur
  end
  
  @current_seed = 0
  def self.current_srand
    @current_seed
  end
  
  def self.current_srand=(val)
    @current_seed = val
  end

  def rand(max=nil)
    max = max.to_i.abs
    x = Platform::POSIX.rand
    # scale result of rand to a domain between 0 and max
    if max.zero?
      x / 0x7fffffff.to_f
    else
      if max < 0x7fffffff
        x / (0x7fffffff / max)
      else
         x * (max / 0x7fffffff)
      end
    end
  end
    
  def lambda
    block = block_given?
    raise ArgumentError, "block required" if block.nil?
    
    block.disable_long_return!
    
    return Proc::Function.from_environment(block)
  end
  alias_method :proc, :lambda

  def caller(start=1)
    return MethodContext.current.sender.calling_hierarchy(start)
  end
  
  def global_variables
    Globals.variables.map { |i| i.to_s }
  end
  
  def loop
    raise LocalJumpError, "no block given" unless block_given?
    
    while true
      yield
    end
  end

  # Sleeps the current thread for +duration+ seconds.
  def sleep(duration = Undefined)
    start = Time.now
    chan = Channel.new
    # No duration means we sleep forever. By not registering anything with
    # Scheduler, the receive call will effectively block until someone
    # explicitely wakes this thread.
    unless duration.equal?(Undefined)
      raise TypeError, 'time interval must be a numeric value' unless duration.kind_of?(Numeric)
      duration = Time.at duration
      Scheduler.send_in_microseconds(chan, (duration.to_f * 1_000_000).to_i)
    end
    chan.receive
    return (Time.now - start).round
  end
  
  
  def at_exit(&block)
    Rubinius::AtExit.unshift(block)
  end

  def to_a
    if self.kind_of? Array
      self
    else
      [self]
    end
  end
  
  def trap(sig, prc=nil, &block)
    Signal.trap(sig, prc, &block)
  end

  def self.after_loaded
    # This nukes the bootstrap raise so the Kernel one is used.
    Object.method_table.delete :raise
    
    get = proc { $! ? $!.backtrace.to_mri : nil }
    set = proc { raise RuntimeError, "illegal setting of backtrace" }
    Globals.set_hook(:$@, get, set)
  end
end

Object.include Kernel

class SystemExit < Exception
  def initialize(code)
    @code = code
  end
  
  attr_reader :code
  
  def message
    "System is exiting with code '#{code}'"
  end
end
  

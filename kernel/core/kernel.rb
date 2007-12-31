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
  module_function :Float
  
  def Integer(obj)
    return obj.to_inum(0, true) if obj.is_a?(String)
    method = obj.respond_to?(:to_int) ? :to_int : :to_i
    Type.coerce_to(obj, Integer, method)
  end
  module_function :Integer

  def Array(obj)
    if obj.respond_to?(:to_ary)
      Type.coerce_to(obj, Array, :to_ary)
    elsif obj.respond_to?(:to_a)
      Type.coerce_to(obj, Array, :to_a)
    else
      [obj]
    end
  end
  module_function :Array

  def String(obj)
    Type.coerce_to(obj, String, :to_s)
  end
  module_function :String
  
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
  module_function :raise
  module_function :fail
  
  def warn(warning)
    unless $VERBOSE.nil?
      $stderr.write warning
      $stderr.write "\n"
    end
    nil
  end
  module_function :warn

  def exit(code=0)
    code = 0 if code.equal? true
    raise SystemExit.new(code)
  end
  module_function :exit
  
  def exit!(code=0)
    Process.exit(code)
  end
  module_function :exit!
  
  def abort(msg=nil)
    Process.abort(msg)
  end  
  module_function :abort
  
  # Display methods
  def printf(target, *args)
    if target.kind_of? IO
      target.printf(*args)
    elsif target.kind_of? String
      $stdout << Sprintf.new(target, *args).parse
    else
      raise TypeError, "The first arg to printf should be an IO or a String"
    end
    nil
  end
  module_function :printf

  def sprintf(str, *args)
    Sprintf.new(str, *args).parse    
  end
  alias_method :format, :sprintf
  module_function :sprintf
  module_function :abort
  
  def puts(*a)
    $stdout.puts(*a)
    return nil
  end
  module_function :puts
  
  def p(*a)
    a = [nil] if a.empty?
    a.each { |obj| $stdout.puts obj.inspect }
    nil
  end
  module_function :p

  def print(*args)
    args.each do |obj|
      $stdout.write obj.to_s
    end
    nil
  end
  module_function :print
    
  def open(path, *rest, &block)
    path = StringValue(path)

    if path.kind_of? String and path.prefix? '|'
      return IO.popen(path[1..-1], *rest, &block)
    end
 
    File.open(path, *rest, &block)
  end
  module_function :open

  # NOTE - this isn't quite MRI compatible.
  # we don't seed the RNG by default with a combination
  # of time, pid and sequence number
  def srand(seed)
    cur = Kernel.current_srand
    Platform::POSIX.srand(seed.to_i)
    Kernel.current_srand = seed.to_i
    return cur
  end
  module_function :srand
  
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
  module_function :rand
    
  def lambda
    block = block_given?
    raise ArgumentError, "block required" if block.nil?
    
    block.disable_long_return!
    
    return Proc::Function.from_environment(block)
  end
  alias_method :proc, :lambda
  module_function :lambda
  module_function :proc

  def caller(start=1)
    return MethodContext.current.sender.calling_hierarchy(start)
  end
  module_function :caller
  
  def global_variables
    Globals.variables.map { |i| i.to_s }
  end
  module_function :global_variables
  
  def loop
    raise LocalJumpError, "no block given" unless block_given?
    
    while true
      yield
    end
  end
  module_function :loop

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
      Scheduler.send_in_seconds(chan, duration.to_f)
    end
    chan.receive
    return (Time.now - start).round
  end
  module_function :sleep
  
  
  def at_exit(&block)
    Rubinius::AtExit.unshift(block)
  end
  module_function :at_exit
  
  def test(cmd, file1, file2=nil)
    case cmd
    when ?d
      File.directory? file1
    when ?e
      File.exist? file1
    when ?f
      File.file? file1
    else
      false
    end
  end
  module_function :test

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
  module_function :trap

  # Activates the singleton +Debugger+ instance, and sets a breakpoint
  # immediately after the call site to this method.
  # TODO: Have method take an options hash to configure debugger behavior,
  # and perhaps a block containing debugger commands to be executed when the
  # breakpoint is hit.
  def debugger
    require 'debugger/debugger'
    # Get hold of debugger, initialising it if not already running
    dbg = Debugger.instance

    # Register a breakpoint immediately following the call site
    ctxt = MethodContext.current.sender
    bp = dbg.get_breakpoint(ctxt.method, ctxt.ip)
    if bp
      unless bp.enabled?
        bp.enable
        ctxt.reload_method
      end
    else
      dbg.set_breakpoint ctxt.method, ctxt.ip
      ctxt.reload_method
    end

    # Have debugger spin up a thread and wait until a breakpoint is hit
    Thread.pass until dbg.waiting_for_breakpoint?
  end
  alias_method :breakpoint, :debugger

  def self.after_loaded
    # This nukes the bootstrap raise so the Kernel one is used.
    Object.method_table.delete :raise
  
    # Add in $! in as a hook, to just do $!. This is for accesses to $!
    # that the compiler can't see.
    get = proc { $! }
    Globals.set_hook(:$!, get, nil)

    # Same as $!, for any accesses we might miss.
    # HACK. I doubt this is correct, because of how it will be called.
    get = proc { Regex.last_match }
    Globals.set_hook(:$~, get, nil)

    get = proc { ARGV }
    Globals.set_hook(:$*, get, nil)

    get = proc { $! ? $!.backtrace : nil }
    Globals.set_hook(:$@, get, nil)

    get = proc { Process.pid }
    Globals.set_hook(:$$, get, nil)
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
  

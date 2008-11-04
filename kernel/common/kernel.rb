# depends on: module.rb kernel.rb misc.rb exception.rb global.rb

##
# Namespace for coercion functions between various ruby objects.

module Type

  ##
  # Returns an object of given class. If given object already is one, it is
  # returned. Otherwise tries obj.meth and returns the result if it is of the
  # right kind. TypeErrors are raised if the conversion method fails or the
  # conversion result is wrong.
  #
  # Uses Type.obj_kind_of to bypass type check overrides.
  #
  # Equivalent to MRI's rb_convert_type().

  def self.coerce_to(obj, cls, meth)
    return obj if self.obj_kind_of?(obj, cls)

    begin
      ret = obj.__send__(meth)
    rescue Exception => e
      raise TypeError, "Coercion error: #{obj.inspect}.#{meth} => #{cls} failed:\n" \
                       "(#{e.message})"
    end

    return ret if self.obj_kind_of?(ret, cls)

    raise TypeError, "Coercion error: obj.#{meth} did NOT return a #{cls} (was #{ret.class})"
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
    if obj.is_a?(String)
      if obj == ''
        raise ArgumentError, "invalid value for Integer: (empty string)"
      else
        return obj.to_inum(0, true)
      end
    end
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

  ##
  # MRI uses a macro named StringValue which has essentially the same
  # semantics as obj.coerce_to(String, :to_str), but rather than using that
  # long construction everywhere, we define a private method similar to
  # String().
  #
  # Another possibility would be to change String() as follows:
  #
  #   String(obj, sym=:to_s)
  #
  # and use String(obj, :to_str) instead of StringValue(obj)

  def StringValue(obj)
    Type.coerce_to(obj, String, :to_str)
  end
  private :StringValue

  ##
  # MRI uses a macro named NUM2DBL which has essentially the same semantics as
  # Float(), with the difference that it raises a TypeError and not a
  # ArgumentError. It is only used in a few places (in MRI and Rubinius).
  #--
  # If we can, we should probably get rid of this.

  def FloatValue(obj)
    begin
      Float(obj)
    rescue
      raise TypeError, 'no implicit conversion to float'
    end
  end
  private :FloatValue

  alias_method :primitive_clone, :clone

  def clone
    copy = primitive_clone
    copy.send :initialize_copy, self
    copy
  end

  alias_method :primitive_dup, :dup

  def dup
    copy = primitive_dup
    copy.send :initialize_copy, self
    copy
  end

  def initialize_copy(source)
    unless source.class == self.class then
      raise TypeError, "initialize_copy should take same class object"
    end
  end
  private :initialize_copy

  ##
  #--
  # HACK todo handle cascading raises (ie, TypeError raise
  # raising forever blows)
  #++

  def raise(exc=Undefined, msg=nil, trace=nil)
    skip = false
    if exc.equal? Undefined
      exc = $!
      if exc
        skip = true
      else
        exc = RuntimeError.new("No current exception")
      end
    elsif exc.respond_to? :exception
      exc = exc.exception msg
      raise ::TypeError, 'exception class/object expected' unless exc.kind_of?(::Exception)
      exc.set_backtrace trace if trace
    elsif exc.kind_of? String or !exc
      exc = ::RuntimeError.exception exc
    else
      raise ::TypeError, 'exception class/object expected'
    end

    if $DEBUG and $VERBOSE != nil
      sender = MethodContext.current.sender
      STDERR.puts "Exception: `#{exc.class}' #{sender.location} - #{exc.message}"
    end

    if !skip and !exc.context
      exc.context = MethodContext.current.sender
    end

    Rubinius.asm(exc) { |e| e.bytecode(self); raise_exc }
  end
  module_function :raise

  alias_method :fail, :raise
  module_function :fail

  def warn(warning)
    $stderr.write "#{warning}\n" unless $VERBOSE.nil?
    nil
  end
  module_function :warn

  def exit(code=0)
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
  module_function :format
  module_function :abort

  def puts(*a)
    $stdout.puts(*a)
    return nil
  end
  module_function :puts

  # For each object given, prints obj.inspect followed by the
  # system record separator to standard output (thus, separator
  # cannot be overridden.) Prints nothing if no objects given.
  def p(*a)
    return nil if a.empty?
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

  #--
  # NOTE: This isn't quite MRI compatible.
  # We don't seed the RNG by default with a combination of time, pid and
  # sequence number
  #++

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

  def endian?(order)
    order == Rubinius::ENDIAN
  end
  module_function :endian?

  def block_given?
    if MethodContext.current.sender.block
      return true
    end

    return false
  end
  module_function :block_given?

  alias_method :iterator?, :block_given?
  module_function :iterator?

  def lambda
    block = block_given?
    raise ArgumentError, "block required" if block.nil?

    block.disable_long_return!

    return Proc::Function.__from_block__(block)
  end
  alias_method :proc, :lambda
  module_function :lambda
  module_function :proc

  def caller(start=1)
    frame = MethodContext.current.sender
    frame.stack_trace_starting_at(start)
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

  #
  # Sleeps the current thread for +duration+ seconds.
  #
  def sleep(duration = Undefined)
    unless duration.equal?(Undefined) or duration.kind_of?(Numeric)
      raise TypeError, 'time interval must be a numeric value'
    end

    start = Time.now

    Thread.sleep duration

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

  def trap(sig, prc=nil, &block)
    Signal.trap(sig, prc, &block)
  end
  module_function :trap

  alias_method :__id__, :object_id

  alias_method :==,   :equal?

  # The "sorta" operator, also known as the case equality operator.
  # Generally while #eql? and #== are stricter, #=== is often used
  # to denote an acceptable match or inclusion. It returns true if
  # the match is considered to be valid and false otherwise. It has
  # one special purpose: it is the operator used by the case expression.
  # So in this expression:
  #
  #   case obj
  #   when /Foo/
  #     ...
  #   when "Hi"
  #     ...
  #   end
  #
  # What really happens is that `/Foo/ === obj` is attempted and so
  # on down until a match is found or the expression ends. The use
  # by Regexp is very illustrative: while obj may satisfy the pattern,
  # it may not be the only option.
  #
  # The default #=== operator checks if the other object is #equal?
  # to this one (i.e., is the same object) or if #== returns true.
  # If neither is true, false is returned instead. Many classes opt
  # to override this behaviour to take advantage of its use in a
  # case expression and to implement more relaxed matching semantics.
  # Notably, the above Regexp as well as String, Module and many others.
  def ===(other)
    equal?(other) || self == other
  end

  ##
  # Regexp matching fails by default but may be overridden by subclasses,
  # notably Regexp and String.

  def =~(other)
    false
  end

  def class_variable_get(sym)
    self.class.class_variable_get sym
  end

  def class_variable_set(sym, value)
    self.class.class_variable_set sym, value
  end

  def class_variables(symbols = false)
    self.class.class_variables(symbols)
  end

  ##
  # \_\_const_set__ is emitted by the compiler for const assignment
  # in userland.
  #
  # This is the catch-all version for unwanted values

  def __const_set__(name, obj)
    raise TypeError, "#{self} is not a class/module"
  end

  ##
  # Activates the singleton Debugger instance, and sets a breakpoint
  # immediately after the call site to this method.
  #--
  # TODO: Have method take an options hash to configure debugger behavior,
  # and perhaps a block containing debugger commands to be executed when the
  # breakpoint is hit.

  def debugger
    require 'debugger/debugger'
    dbg = Debugger.instance

    unless dbg.interface
      # Default to command-line interface if nothing registered
      require 'debugger/interface'
      Debugger::CmdLineInterface.new
    end

    ctxt = MethodContext.current.sender
    cm = ctxt.method
    ip = ctxt.ip
    bp = dbg.get_breakpoint(cm, ip)
    if bp
      bp.enable unless bp.enabled?
    else
      bp = dbg.set_breakpoint(cm, ip)
    end

    # Modify send site not to call this method again
    bc = ctxt.method.iseq
    
    Breakpoint.encoder.replace_instruction(bc, ip-4, [:noop])
    Breakpoint.encoder.replace_instruction(bc, ip-2, [:noop])

    ctxt.reload_method
  end

  alias_method :breakpoint, :debugger

  def extend(*modules)
    modules.reverse_each do |mod|
      mod.extend_object(self)
      mod.send(:extended, self)
    end
    self
  end

  def inspect
    return "..." if RecursionGuard.inspecting? self

    ivars = instance_variables

    prefix = "#{self.class}:0x#{self.object_id.to_s(16)}"
    parts = []

    RecursionGuard.inspect self do
      ivars.each do |var|
        parts << "#{var}=#{instance_variable_get(var).inspect}"
      end
    end

    if parts.empty?
      "#<#{prefix}>"
    else
      "#<#{prefix} #{parts.join(' ')}>"
    end
  end

  ##
  # :call-seq:
  #   obj.instance_exec(arg, ...) { |var,...| block }  => obj
  #
  # Executes the given block within the context of the receiver +obj+. In
  # order to set the context, the variable +self+ is set to +obj+ while the
  # code is executing, giving the code access to +obj+'s instance variables.
  #
  # Arguments are passed as block parameters.
  #
  #   class Klass
  #     def initialize
  #       @secret = 99
  #     end
  #   end
  #   
  #   k = Klass.new
  #   k.instance_exec(5) {|x| @secret+x }   #=> 104

  def instance_exec(*args, &prc)
    raise ArgumentError, "Missing block" unless block_given?
    env = prc.block.redirect_to self
    env.method.scope = StaticScope.new(metaclass, env.method.scope)
    env.call(*args)
  end

  ##
  # Returns true if this object is an instance of the given class, otherwise
  # false. Raises a TypeError if a non-Class object given.
  #
  # Module objects can also be given for MRI compatibility but the result is
  # always false.

  def instance_of?(cls)
    if cls.class != Class and cls.class != Module
      # We can obviously compare against Modules but result is always false
      raise TypeError, "instance_of? requires a Class argument"
    end

    self.class == cls
  end

  def instance_variable_get(sym)
    sym = instance_variable_validate(sym)
    get_instance_variable(sym)
  end

  def instance_variable_set(sym, value)
    sym = instance_variable_validate(sym)
    set_instance_variable(sym, value)
  end

  def remove_instance_variable(sym)
    # HACK
    instance_variable_set(sym, nil)
  end
  private :remove_instance_variable

  def instance_variables
    vars = get_instance_variables
    return [] unless vars

    return vars.keys.collect { |v| v.to_s }
  end

  def instance_variable_defined?(name)
    name = instance_variable_validate(name)

    vars = get_instance_variables
    return false unless vars

    name = name.to_sym unless name.kind_of? Symbol

    return vars.key?(name)
  end

  # Both of these are for defined? when used inside a proxy obj that
  # may undef the regular method. The compiler generates __ calls.
  alias_method :__instance_variable_defined_eh__, :instance_variable_defined?
  alias_method :__respond_to_eh__, :respond_to?

  def singleton_method_added(name)
  end
  private :singleton_method_added

  def singleton_method_removed(name)
  end
  private :singleton_method_removed

  def singleton_method_undefined(name)
  end
  private :singleton_method_undefined

  alias_method :is_a?, :kind_of?

  def method(name)
    cm = __find_method__(name)

    if cm
      return Method.new(self, cm[1], cm[0])
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end

  def nil?
    false
  end

  def methods(all=true)
    names = singleton_methods(all)
    names |= self.class.instance_methods(true) if all
    return names
  end

  def private_methods(all=true)
    names = private_singleton_methods
    names |= self.class.private_instance_methods(all)
    return names
  end

  def private_singleton_methods
    metaclass.method_table.private_names.map { |meth| meth.to_s }
  end

  def protected_methods(all=true)
    names = protected_singleton_methods
    names |= self.class.protected_instance_methods(all)
    return names
  end

  def protected_singleton_methods
    metaclass.method_table.protected_names.map { |meth| meth.to_s }
  end

  def public_methods(all=true)
    names = singleton_methods(all)
    names |= self.class.public_instance_methods(all)
    return names
  end

  def singleton_methods(all=true)
    mt = metaclass.method_table
    if all
      return mt.keys.map { |m| m.to_s }
    else
      (mt.public_names + mt.protected_names).map { |m| m.to_s }
    end
  end

  alias_method :send, :__send__

  def to_a
    if self.kind_of? Array
      self
    else
      [self]
    end
  end

  def to_s
    "#<#{self.__class__}:0x#{self.__id__.to_s(16)}>"
  end

  def autoload(name, file)
    Object.autoload(name, file)
  end
  private :autoload

  def autoload?(name)
    Object.autoload?(name)
  end
  private :autoload?

  def set_trace_func(*args)
    raise NotImplementedError
  end
  module_function :set_trace_func
  
  def syscall(*args)
    raise NotImplementedError
  end
  module_function :syscall

  def trace_var(*args)
    raise NotImplementedError
  end
  module_function :trace_var

  def untrace_var(*args)
    raise NotImplementedError
  end
  module_function :untrace_var

  # Perlisms.

  def chomp(string=$/)
    ensure_last_read_string
    $_ = $_.chomp(string)
  end
  module_function :chomp
  
  def chomp!(string=$/)
    ensure_last_read_string
    $_.chomp!(string)
  end
  module_function :chomp!

  def chop(string=$/)
    ensure_last_read_string
    $_ = $_.chop(string)
  end
  module_function :chop
  
  def chop!(string=$/)
    ensure_last_read_string
    $_.chop!(string)
  end
  module_function :chop!

  def getc
    $stdin.getc
  end
  module_function :getc

  def putc(int)
    $stdin.putc(int)
  end
  module_function :putc

  def gets(sep=$/)
    # HACK. Needs to use ARGF first.
    $stdin.gets(sep)
  end
  module_function :gets

  def readline(sep)
    $stdin.readline(sep)
  end
  module_function :readline
  
  def readlines(sep)
    $stdin.readlines(sep)
  end
  module_function :readlines

  def gsub(pattern, rep=nil, &block)
    ensure_last_read_string
    $_ = $_.gsub(pattern, rep, &block)
  end
  module_function :gsub
  
  def gsub!(pattern, rep=nil, &block)
    ensure_last_read_string
    $_.gsub!(pattern, rep, &block)
  end
  module_function :gsub!

  def sub(pattern, rep=nil, &block)
    ensure_last_read_string
    $_ = $_.sub(pattern, rep, &block)
  end
  module_function :sub
  
  def sub!(pattern, rep=nil, &block)
    ensure_last_read_string
    $_.sub!(pattern, rep, &block)
  end
  module_function :sub!

  def scan(pattern, &block)
    ensure_last_read_string
    $_.scan(pattern, &block)
  end
  module_function :scan

  def select(*args)
    IO.select(*args)
  end
  module_function :select

  def split(*args)
    ensure_last_read_string
    $_.split(*args)
  end
  module_function :split

  # Checks whether the "last read line" $_ variable is a String,
  # raising a TypeError when not.
  def ensure_last_read_string
    unless $_.kind_of? String
      cls = $_.nil? ? "nil" : $_.class
      raise TypeError, "$_ must be a String (#{cls} given)"
    end
  end
  module_function :ensure_last_read_string
  private :ensure_last_read_string

end

class SystemExit < Exception

  ##
  # Process exit status if this exception is raised

  attr_reader :status

  ##
  # Creates a SystemExit exception with optional status and message.  If the
  # status is omitted, Process::EXIT_SUCCESS is used.

  def initialize(status = Process::EXIT_SUCCESS)
    @status = Type.coerce_to status, Fixnum, :to_int
  end
end


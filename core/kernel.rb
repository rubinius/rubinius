module Kernel
  def equal?(other)
    Rubinius.primitive :object_equal
    raise PrimitiveFailure, "Kernel#equal? primitive failed"
  end

  alias_method :eql?, :equal?
  alias_method :==,  :equal?
  alias_method :===, :equal?

  def !~(other)
    res = self =~ other ? false : true
    Regexp.last_match = Regexp.last_match
    res
  end

  def singleton_class
    Rubinius::Type.object_singleton_class self
  end

  def singleton_class?
    !!Rubinius::Type.singleton_class_object(self)
  end

  def extend(mod)
    Rubinius::Type.object_singleton_class(self).include(mod)
    self
  end

  def freeze
    Rubinius.primitive :object_freeze
    raise PrimitiveFailure, "Kernel#freeze primitive failed"
  end

  def frozen?
    Rubinius.primitive :object_frozen_p
    raise PrimitiveFailure, "Kernel#frozen? primitive failed"
  end

  def hash
    Rubinius.primitive :object_hash
    raise PrimitiveFailure, "Kernel#hash primitive failed"
  end

  def inspect
    "#<#{self.class.name}"
  end

  def respond_to_prim?(meth, visibility)
    Rubinius.primitive :object_respond_to
    meth = Rubinius::Type.coerce_to_symbol meth
    respond_to_prim?(meth, visibility)
  end

  private :respond_to_prim?

  def taint
    Rubinius.primitive :object_taint
    raise PrimitiveFailure, "Kernel#taint primitive failed"
  end

  def tainted?
    Rubinius.primitive :object_tainted_p
    raise PrimitiveFailure, "Kernel#tainted? primitive failed"
  end

  def untaint
    Rubinius.primitive :object_untaint
    raise PrimitiveFailure, "Kernel#untaint primitive failed"
  end

  alias_method :untrust, :taint
  alias_method :trust, :untaint
  alias_method :untrusted?, :tainted?

  # NOTE: The bootstrap method used to add method definitions to the class
  # method_table still returns a CompiledCode instance, so this chaining
  # works.
  #
  # TODO: Fix this chaining by introducing a proper facility to operate on
  # methods.
  def respond_to?(meth, include_private=false)
    respond_to_prim?(meth, include_private)
  end

  custom_call_site :respond_to?

  def respond_to_missing?(meth, include_private)
    false
  end
  private :respond_to_missing?

  def yield_gdb(obj)
    Rubinius.primitive :yield_gdb
    raise PrimitiveFailure, "Kernel#yield_gdb primitive failed"
  end
  private :yield_gdb

  def Array(obj)
    ary = Rubinius::Type.check_convert_type obj, Array, :to_ary

    return ary if ary

    if array = Rubinius::Type.check_convert_type(obj, Array, :to_a)
      array
    else
      [obj]
    end
  end
  module_function :Array

  def Complex(*args)
    Rubinius.privately do
      Complex.convert(*args)
    end
  end
  module_function :Complex

  def Float(obj)
    case obj
    when String
      Rubinius::Type.coerce_string_to_float obj, true
    else
      Rubinius::Type.coerce_object_to_float obj
    end
  end
  module_function :Float

  ##
  # MRI uses a macro named NUM2DBL which has essentially the same semantics as
  # Float(), with the difference that it raises a TypeError and not a
  # ArgumentError. It is only used in a few places (in MRI and Rubinius).
  #--
  # If we can, we should probably get rid of this.

  def FloatValue(obj)
    exception = TypeError.new 'no implicit conversion to float'

    case obj
    when String
      raise exception
    else
      begin
        Rubinius::Type.coerce_object_to_float obj
      rescue
        raise exception
      end
    end
  end
  private :FloatValue

  def Hash(obj)
    return {} if obj.nil? || obj == []

    if hash = Rubinius::Type.check_convert_type(obj, Hash, :to_hash)
      return hash
    end

    raise TypeError, "can't convert #{obj.class} into Hash"
  end
  module_function :Hash

  def Integer(obj, base=nil)
    if obj.kind_of? String
      if obj.empty?
        raise ArgumentError, "invalid value for Integer: (empty string)"
      else
        base ||= 0
        return obj.to_inum(base, true)
      end
    end

    if base
      raise ArgumentError, "base is only valid for String values"
    end

    case obj
    when Integer
      obj
    when Float
      if obj.nan? or obj.infinite?
        raise FloatDomainError, "unable to coerce #{obj} to Integer"
      else
        obj.to_int
      end
    when NilClass
      raise TypeError, "can't convert nil into Integer"
    else
      # Can't use coerce_to or try_convert because I think there is an
      # MRI bug here where it will return the value without checking
      # the return type.
      if obj.respond_to? :to_int
        if val = obj.to_int
          return val
        end
      end

      Rubinius::Type.coerce_to obj, Integer, :to_i
    end
  end
  module_function :Integer

  def Rational(a, b = 1)
    Rubinius.privately do
      Rational.convert a, b
    end
  end
  module_function :Rational

  def String(obj)
    return obj if obj.kind_of? String

    unless obj.respond_to?(:to_s)
      raise TypeError, "can't convert #{obj.class} into String"
    end

    begin
      str = obj.to_s
    rescue NoMethodError
      raise TypeError, "can't convert #{obj.class} into String"
    end

    unless str.kind_of? String
      raise TypeError, "#to_s did not return a String"
    end

    return str
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
    Rubinius::Type.coerce_to obj, String, :to_str
  end
  module_function :StringValue

  def __method__
    scope = Rubinius::VariableScope.of_sender

    name = scope.method.name

    return nil if scope.method.for_module_body?
    # If the name is still __block__, then it's in a script, so return nil
    return nil if name == :__block__ or name == :__script__
    name
  end
  module_function :__method__

  alias_method :__callee__, :__method__
  module_function :__callee__

  def __dir__
    scope = Rubinius::LexicalScope.of_sender
    script = scope.current_script
    basepath = script.file_path
    fullpath = nil

    return nil unless basepath

    fullpath = if script.data_path
      script.data_path
    else
      Rubinius.privately do
        File.basic_realpath(basepath)
      end
    end

    File.dirname fullpath
  end
  module_function :__dir__

  def `(str) #`
    str = StringValue(str) unless str.kind_of?(String)
    pid, output = Rubinius::Mirror::Process.backtick str
    Process.waitpid(pid)

    Rubinius::Type.external_string output
  end
  module_function :` # `

  def =~(other)
    nil
  end

  def <=>(other)
    self == other ? 0 : nil
  end

  def ===(other)
    equal?(other) || self == other
  end

  def itself
    self
  end

  def abort(msg=nil)
    Process.abort msg
  end
  module_function :abort

  def at_exit(prc=nil, &block)
    if prc
      unless prc.respond_to?(:call)
        raise "Argument must respond to #call"
      end
    else
      prc = block
    end

    unless prc
      raise "must pass a #call'able or block"
    end

    Rubinius::AtExit.unshift(prc)
  end
  module_function :at_exit

  def autoload(name, file)
    Object.autoload(name, file)
  end
  private :autoload

  def autoload?(name)
    Object.autoload?(name)
  end
  private :autoload?

  def block_given?
    Rubinius::VariableScope.of_sender.block != nil
  end
  module_function :block_given?

  alias_method :iterator?, :block_given?
  module_function :iterator?

  def caller_inner(start, length)
    if start.is_a?(Range)
      length = start.size
      start = start.begin
    end

    # The + 2 is to skip this frame and the caller's.
    #
    # TODO: plumb length into mri_backtrace to save some work.
    full_trace = Rubinius.mri_backtrace(start + 2)
    if length
      full_trace = full_trace.slice(0, length)
    end
    full_trace
  end
  module_function :caller_inner
  private :caller_inner

  def caller(start = 1, length = nil)
    caller_inner(start, length).map do |tup|
      code     = tup[0]
      line     = tup[1]
      is_block = tup[2]
      name     = tup[3]

      "#{code.active_path}:#{line}:in `#{name}'"
    end
  end
  module_function :caller

  ##
  # Returns the current call stack as an Array of Thread::Backtrace::Location
  # instances. This method is available starting with Ruby 2.0.
  #
  def caller_locations(start = 1, length = nil)
    caller_inner(start, length).map do |tup|
      scope    = tup[0].scope
      abs_path = tup[0].active_path
      path     = scope ? scope.active_path : abs_path
      label    = tup[3].to_s

      Thread::Backtrace::Location.new(label, abs_path, path, tup[1])
    end
  end
  module_function :caller_locations

  def define_singleton_method(*args, &block)
    singleton_class.send(:define_method, *args, &block)
  end

  def display(port=$>)
    port.write self
  end

  def exec(*args)
    Process.exec(*args)
  end
  module_function :exec

  def exit(code=0)
    Process.exit(code)
  end
  module_function :exit

  def exit!(code=1)
    Process.exit!(code)
  end
  module_function :exit!

  def fork(&block)
    Process.fork(&block)
  end
  module_function :fork

  def getc
    $stdin.getc
  end
  module_function :getc

  def gets(sep=$/)
    ARGF.gets(sep)
  end
  module_function :gets

  def global_variables
    Rubinius::Globals.variables
  end
  module_function :global_variables

  def initialize_dup(other)
    initialize_copy(other)
  end
  private :initialize_dup

  def initialize_clone(other)
    initialize_copy(other)
  end
  private :initialize_clone

  def initialize_copy(source)
    unless instance_of?(Rubinius::Type.object_class(source))
      raise TypeError, "initialize_copy should take same class object"
    end
  end
  private :initialize_copy

  def inspect
    prefix = "#<#{self.class}:0x#{self.__id__.to_s(16)}"

    # The protocol here seems odd, but it's to match MRI.
    #
    # MRI side-calls to the C function that implements Kernel#to_s. If that
    # method is overridden, the new Ruby method is never called. So, we inline
    # the code for Kernel#to_s here because we simply dispatch to Ruby
    # methods.
    ivars = Rubinius.invoke_primitive :object_instance_variables, self

    if ivars.empty?
      return Rubinius::Type.infect "#{prefix}>", self
    end

    # Otherwise, if it's already been inspected, return the ...
    return "#{prefix} ...>" if Thread.guarding? self

    # Otherwise, gather the ivars and show them.
    parts = []

    Thread.recursion_guard self do
      ivars.each do |var|
        parts << "#{var}=#{__instance_variable_get__(var).inspect}"
      end
    end

    if parts.empty?
      str = "#{prefix}>"
    else
      str = "#{prefix} #{parts.join(' ')}>"
    end

    Rubinius::Type.infect(str, self)

    return str
  end

  ##
  # Returns true if this object is an instance of the given class, otherwise
  # false. Raises a TypeError if a non-Class object given.
  #
  # Module objects can also be given for MRI compatibility but the result is
  # always false.

  def instance_of?(cls)
    Rubinius.primitive :object_instance_of

    arg_class = Rubinius::Type.object_class(cls)
    if arg_class != Class and arg_class != Module
      # We can obviously compare against Modules but result is always false
      raise TypeError, "instance_of? requires a Class argument"
    end

    Rubinius::Type.object_class(self) == cls
  end

  def instance_variable_get(sym)
    Rubinius.primitive :object_get_ivar

    sym = Rubinius::Type.ivar_validate sym
    instance_variable_get sym
  end

  alias_method :__instance_variable_get__, :instance_variable_get

  def instance_variable_set(sym, value)
    Rubinius.primitive :object_set_ivar

    sym = Rubinius::Type.ivar_validate sym
    instance_variable_set sym, value
  end

  alias_method :__instance_variable_set__, :instance_variable_set

  def instance_variables
    Rubinius.invoke_primitive :object_instance_variables, self
  end

  def instance_variable_defined?(name)
    Rubinius.primitive :object_ivar_defined

    instance_variable_defined? Rubinius::Type.ivar_validate(name)
  end

  # Both of these are for defined? when used inside a proxy obj that
  # may undef the regular method. The compiler generates __ calls.
  alias_method :__instance_variable_defined_p__, :instance_variable_defined?
  alias_method :__respond_to_p__, :respond_to?

  alias_method :is_a?, :kind_of?

  def lambda
    env = nil

    Rubinius.asm do
      push_block
      # assign a pushed block to the above local variable "env"
      # Note that "env" is indexed at 0.
      set_local 0
    end

    raise ArgumentError, "block required" unless env

    prc = Rubinius::Mirror::Proc.from_block ::Proc, env

    # Make a proc lambda only when passed an actual block (ie, not using the
    # "&block" notation), otherwise don't modify it at all.
    prc.lambda_style! if env.is_a?(Rubinius::BlockEnvironment)

    return prc
  end
  module_function :lambda

  def load(name, wrap=false)
    cl = Rubinius::CodeLoader.new(name)
    cl.load(wrap)

    Rubinius.run_script cl.compiled_code

    Rubinius::CodeLoader.loaded_hook.trigger!(name)

    return true
  end
  module_function :load

  def loop
    return to_enum(:loop) { Float::INFINITY } unless block_given?

    begin
      while true
        yield
      end
    rescue StopIteration => e
      e.result
    end
  end
  module_function :loop

  def method(name)
    name = Rubinius::Type.coerce_to_symbol name
    code = Rubinius.find_method(self, name)

    if code
      Method.new(self, code[1], code[0], name)
    elsif respond_to_missing?(name, true)
      Method.new(self, self.class, Rubinius::MissingMethod.new(self,  name), name)
    else
      raise NameError, "undefined method `#{name}' for class #{self.class}"
    end
  end

  def methods(all=true)
    methods = singleton_methods(all)

    if all
      # We have to special case these because unlike true, false, nil,
      # Type.object_singleton_class raises a TypeError.
      case self
      when Fixnum, Symbol
        methods |= Rubinius::Type.object_class(self).instance_methods(true)
      else
        methods |= Rubinius::Type.object_singleton_class(self).instance_methods(true)
      end
    end

    return methods if kind_of?(ImmediateValue)

    undefs = []
    Rubinius::Type.object_singleton_class(self).method_table.filter_entries do |entry|
      undefs << entry.name.to_s if entry.visibility == :undef
    end

    return methods - undefs
  end

  def nil?
    false
  end

  def object_id
    Rubinius.primitive :object_id
    raise PrimitiveFailure, "Kernel#object_id primitive failed"
  end

  def open(obj, *rest, &block)
    if obj.respond_to?(:to_open)
      obj = obj.to_open(*rest)

      if block_given?
        return yield(obj)
      else
        return obj
      end
    end

    path = Rubinius::Type.coerce_to_path obj

    if path.kind_of? String and path.prefix? '|'
      return IO.popen(path[1..-1], *rest, &block)
    end

    File.open(path, *rest, &block)
  end
  module_function :open

  def p(*a)
    return nil if a.empty?
    a.each { |obj| $stdout.puts obj.inspect }
    $stdout.flush

    a.size == 1 ? a.first : a
  end
  module_function :p

  def print(*args)
    args.each do |obj|
      $stdout.write obj.to_s
    end
    nil
  end
  module_function :print

  def printf(target, *args)
    if target.kind_of?(String)
      output = $stdout
    else
      output = target
      target = args.shift
    end
    output.write Rubinius::Sprinter.get(target).call(*args)
    nil
  end
  module_function :printf

  def private_methods(all=true)
    private_singleton_methods() | Rubinius::Type.object_class(self).private_instance_methods(all)
  end

  def private_singleton_methods
    sc = Rubinius::Type.object_singleton_class self
    methods = sc.method_table.private_names

    m = sc

    while m = m.direct_superclass
      unless Rubinius::Type.object_kind_of?(m, Rubinius::IncludedModule) or
             Rubinius::Type.singleton_class_object(m)
        break
      end

      methods.concat m.method_table.private_names
    end

    methods
  end
  private :private_singleton_methods

  def proc
    env = nil

    Rubinius.asm do
      push_block
      # assign a pushed block to the above local variable "env"
      set_local 0
    end

    unless env
      # Support for ancient pre-block-pass style:
      # def something
      #   proc
      # end
      # something { a_block } => Proc instance
      env = Rubinius::BlockEnvironment.of_sender

      unless env
        raise ArgumentError, "tried to create a Proc object without a block"
      end
    end

    Proc.new(&env)
  end
  module_function :proc

  def protected_methods(all=true)
    protected_singleton_methods() | Rubinius::Type.object_class(self).protected_instance_methods(all)
  end

  def protected_singleton_methods
    m = Rubinius::Type.object_singleton_class self
    methods = m.method_table.protected_names

    while m = m.direct_superclass
      unless Rubinius::Type.object_kind_of?(m, Rubinius::IncludedModule) or
             Rubinius::Type.singleton_class_object(m)
        break
      end

      methods.concat m.method_table.protected_names
    end

    methods
  end
  private :protected_singleton_methods

  def public_method(name)
    name = Rubinius::Type.coerce_to_symbol name
    code = Rubinius.find_public_method(self, name)

    if code
      Method.new(self, code[1], code[0], name)
    elsif respond_to_missing?(name, false)
      Method.new(self, self.class, Rubinius::MissingMethod.new(self,  name), name)
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end

  def public_methods(all=true)
    public_singleton_methods | Rubinius::Type.object_class(self).public_instance_methods(all)
  end

  def public_send(message, *args)
    Rubinius.primitive :object_public_send
    raise PrimitiveFailure, "Kernel#public_send primitive failed"
  end

  def public_singleton_methods
    m = Rubinius::Type.object_singleton_class self
    methods = m.method_table.public_names

    while m = m.direct_superclass
      unless Rubinius::Type.object_kind_of?(m, Rubinius::IncludedModule) or
             Rubinius::Type.singleton_class_object(m)
        break
      end

      methods.concat m.method_table.public_names
    end

    methods
  end
  private :public_singleton_methods

  def putc(int)
    $stdout.putc(int)
  end
  module_function :putc

  def puts(*a)
    $stdout.puts(*a)
    nil
  end
  module_function :puts

  def rand(limit=0)
    if limit == 0
      return Thread.current.randomizer.random_float
    end

    if limit.kind_of?(Range)
      return Thread.current.randomizer.random(limit)
    else
      limit = Integer(limit).abs

      if limit == 0
        Thread.current.randomizer.random_float
      else
        Thread.current.randomizer.random_integer(limit - 1)
      end
    end
  end
  module_function :rand

  def readline(sep=$/)
    ARGF.readline(sep)
  end
  module_function :readline

  def readlines(sep=$/)
    ARGF.readlines(sep)
  end
  module_function :readlines

  def remove_instance_variable(sym)
    Rubinius.primitive :object_del_ivar

    # If it's already a symbol, then we're here because it doesn't exist.
    if sym.kind_of? Symbol
      raise NameError, "instance variable '#{sym}' not defined"
    end

    # Otherwise because sym isn't a symbol, coerce it and try again.
    remove_instance_variable Rubinius::Type.ivar_validate(sym)
  end

  def require(name)
    Rubinius::CodeLoader.require name
  end
  module_function :require

  def require_relative(name)
    scope = Rubinius::LexicalScope.of_sender
    Rubinius::CodeLoader.require_relative(name, scope)
  end
  module_function :require_relative

  def select(*args)
    IO.select(*args)
  end
  module_function :select

  def send(message, *args)
    Rubinius.primitive :object_send
    raise PrimitiveFailure, "Kernel#send primitive failed"
  end

  def set_trace_func(*args)
    raise NotImplementedError
  end
  module_function :set_trace_func

  def sprintf(str, *args)
    Rubinius::Sprinter.get(str).call(*args)
  end
  module_function :sprintf

  alias_method :format, :sprintf
  module_function :format

  def sleep(duration=undefined)
    Rubinius.primitive :vm_sleep

    # The primitive will fail on arg count if sleep is called
    # without an argument, so we call it again passing undefined
    # to mean "sleep forever"
    #
    if undefined.equal? duration
      return sleep(undefined)
    end

    if duration.kind_of? Numeric
      float = Rubinius::Type.coerce_to duration, Float, :to_f
      return sleep(float)
    else
      raise TypeError, 'time interval must be a numeric value'
    end
  end
  module_function :sleep

  def srand(seed=undefined)
    if undefined.equal? seed
      seed = Thread.current.randomizer.generate_seed
    end

    seed = Rubinius::Type.coerce_to seed, Integer, :to_int
    Thread.current.randomizer.swap_seed seed
  end
  module_function :srand

  def tap
    yield self
    self
  end

  def test(cmd, file1, file2=nil)
    case cmd
    when ?d
      File.directory? file1
    when ?e
      File.exist? file1
    when ?f
      File.file? file1
    when ?l
      File.symlink? file1
    when ?r
      File.readable? file1
    when ?R
      File.readable_real? file1
    when ?w
      File.writable? file1
    when ?W
      File.writable_real? file1
    when ?A
      File.atime file1
    when ?C
      File.ctime file1
    when ?M
      File.mtime file1
    else
      raise NotImplementedError, "command ?#{cmd.chr} not implemented"
    end
  end
  module_function :test

  def to_enum(method=:each, *args, &block)
    Enumerator.new(self, method, *args).tap do |enum|
      Rubinius.privately { enum.size = block } if block_given?
    end
  end
  alias_method :enum_for, :to_enum

  def trap(sig, prc=nil, &block)
    Signal.trap(sig, prc, &block)
  end
  module_function :trap

  def singleton_method_added(name)
  end
  private :singleton_method_added

  def singleton_method_removed(name)
  end
  private :singleton_method_removed

  def singleton_method_undefined(name)
  end
  private :singleton_method_undefined

  def singleton_methods(all=true)
    m = Rubinius::Type.object_singleton_class self
    mt = m.method_table
    methods = mt.public_names + mt.protected_names

    if all
      while m = m.direct_superclass
        unless Rubinius::Type.object_kind_of?(m, Rubinius::IncludedModule) or
               Rubinius::Type.singleton_class_object(m)
          break
        end

        mt = m.method_table
        methods.concat mt.public_names
        methods.concat mt.protected_names
      end
    end

    methods.uniq
  end

  def spawn(*args)
    Process.spawn(*args)
  end
  module_function :spawn

  def syscall(*args)
    raise NotImplementedError
  end
  module_function :syscall

  def system(*args)
    begin
      pid = Process.spawn(*args)
    rescue SystemCallError
      return nil
    end

    Process.waitpid pid
    $?.exitstatus == 0
  end
  module_function :system

  def trace_var(name, cmd = nil, &block)
    if !cmd && !block
      raise(
        ArgumentError,
        'The 2nd argument should be a Proc/String, alternatively use a block'
      )
    end

    # We have to use a custom proc since set_hook passes in both the variable
    # name and value.
    set = proc do |_, value|
      if cmd.is_a?(String)
        eval(cmd)

      # In MRI if one passes both a proc in `cmd` and a block the latter will
      # be ignored.
      elsif cmd.is_a?(Proc)
        cmd.call(value)

      elsif block
        block.call(value)
      end
    end

    Rubinius::Globals.set_hook(name, :[], set)
  end
  module_function :trace_var

  # In MRI one can specify a 2nd argument to remove a specific tracer.
  # Rubinius::Globals however only supports one hook per variable, hence the
  # 2nd dummy argument.
  def untrace_var(name, *args)
    Rubinius::Globals.remove_hook(name)
  end
  module_function :untrace_var

  def warn(*messages)
    $stderr.puts(*messages) if !$VERBOSE.nil? && !messages.empty?
    nil
  end
  module_function :warn

  def warning(message)
    $stderr.puts message if $VERBOSE
  end
  module_function :warning

  def catch(obj = Object.new, &block)
    raise LocalJumpError unless block_given?

    Rubinius::ThrownValue.register(obj) do
      return Rubinius.catch(obj, block)
    end
  end
  module_function :catch

  def throw(obj, value=nil)
    unless Rubinius::ThrownValue.available? obj
      exc = UncaughtThrowError.new "uncaught throw #{obj.inspect}"
      exc.tag = obj
      raise exc
    end

    Rubinius.throw obj, value
  end
  module_function :throw

  # Names of local variables at point of call (including evaled)
  #
  def local_variables
    scope = Rubinius::VariableScope.of_sender
    scope.local_variables
  end
  module_function :local_variables

  # Obtain binding here for future evaluation/execution context.
  #
  def binding
    return Binding.setup(
      Rubinius::VariableScope.of_sender,
      Rubinius::CompiledCode.of_sender,
      Rubinius::LexicalScope.of_sender,
      Rubinius::VariableScope.of_sender.self,
      Rubinius::Location.of_closest_ruby_method
    )
  end
  module_function :binding

  # Evaluate and execute code given in the String.
  #
  def eval(string, binding=nil, filename=nil, lineno=nil)
    string = StringValue(string)
    filename = StringValue(filename) if filename
    lineno = Rubinius::Type.coerce_to lineno, Fixnum, :to_i if lineno
    lineno = 1 if filename && !lineno

    if binding
      binding = Rubinius::Type.coerce_to_binding binding
      filename ||= binding.lexical_scope.active_path
    else
      binding = Binding.setup(Rubinius::VariableScope.of_sender,
                              Rubinius::CompiledCode.of_sender,
                              Rubinius::LexicalScope.of_sender,
                              self)

      filename ||= "(eval)"
    end

    lineno ||= binding.line_number

    existing_scope = binding.lexical_scope
    binding.lexical_scope = existing_scope.dup

    c = Rubinius::ToolSets::Runtime::Compiler
    be = c.construct_block string, binding, filename, lineno

    result = be.call_on_instance(binding.receiver)
    binding.lexical_scope = existing_scope
    result
  end
  module_function :eval
  private :eval
end

module Kernel
  def __method__
    # Look in the backtrace for locations where the scope is that of the sender.
    # The first non-block one will be our call,
    # unless where are called from a DelegatedMethod
    # For the later, we check to see if the next location
    # was a call from a delegated method
    easy_name =  Rubinius::VariableScope.of_sender.method.name

    # A toplevel block.
    return nil if easy_name == :__block__

    # __eval__ is weird, use the complicated logic.
    return easy_name unless easy_name == :__eval__

    scope_of_sender = Rubinius::StaticScope.of_sender
    trace = Rubinius::VM.backtrace(1)
    trace.each_with_index do |loc, i|
      next unless loc.method.scope == scope_of_sender
      if loc.is_block
        return loc.name
      else
        return loc.method.name == :__script__ ? nil : loc.method.name
      end
    end
    nil
  end

  def Float(obj)
    raise TypeError, "can't convert nil into Float" if obj.nil?

    if obj.is_a?(Float)
      return obj
    elsif obj.is_a?(String)
      valid_re = /^\s*[+-]?((\d+_?)*\d+(\.(\d+_?)*\d+)?|\.(\d+_?)*\d+)(\s*|([eE][+-]?(\d+_?)*\d+)\s*)$/

      m = valid_re.match(obj)

      if !m or !m.pre_match.empty? or !m.post_match.empty?
        raise ArgumentError, "invalid value for Float(): #{obj.inspect}"
      end
      return obj.convert_float
    end

    coerced_value = Type.coerce_to(obj, Float, :to_f)
    if coerced_value.nan?
      raise ArgumentError, "invalid value for Float(): #{coerced_value.inspect}"
    end
    coerced_value
  end
  module_function :Float

  def Integer(obj)
    if obj.is_a? String
      if obj == ''
        raise ArgumentError, "invalid value for Integer: (empty string)"
      else
        return obj.to_inum(0, true)
      end
    elsif obj.is_a? Float
      if obj.nan? or obj.infinite?
        raise FloatDomainError, "unable to coerce #{obj} to Integer"
      else
        return obj.to_int
      end
    elsif obj.respond_to? :to_int
      int_value = obj.to_int
      return int_value unless int_value.nil?
    end

    Type.coerce_to(obj, Integer, :to_i)
  end
  module_function :Integer

  def Array(obj)
    if obj.respond_to? :to_ary
      ary = Type.convert_to obj, Array, :to_ary
    end

    return ary unless ary.equal? nil

    if obj.respond_to? :to_a
      Type.coerce_to(obj, Array, :to_a)
    else
      [obj]
    end
  end
  module_function :Array

  def String(obj)
    if obj.is_a? String
      return obj
    elsif obj.respond_to?(:to_s)
      coerced_str = obj.to_s
      return coerced_str if coerced_str.is_a? String
      raise TypeError, "Coercion error: obj.to_s did NOT return a String (was #{coerced_str.class})"
    else
      raise TypeError, "can't convert Object into String: #{obj.inspect}"
    end
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

  def initialize_copy(source)
    unless source.class == self.class then
      raise TypeError, "initialize_copy should take same class object"
    end
  end
  private :initialize_copy

  def warn(warning)
    $stderr.write "#{warning}\n" unless $VERBOSE.nil?
    nil
  end
  module_function :warn

  def warning(message)
    $stderr.puts message if $VERBOSE
  end
  module_function :warning

  def exit(code=0)
    Process.exit(code)
  end
  module_function :exit

  def exit!(code=1)
    Process.exit!(code)
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
      $stdout << Rubinius::Sprinter.get(target).call(*args)
    else
      raise TypeError, "The first arg to printf should be an IO or a String"
    end
    nil
  end
  module_function :printf

  def sprintf(str, *args)
    ::Rubinius::Sprinter.get(str).call(*args)
  end
  alias_method :format, :sprintf
  module_function :sprintf
  module_function :format

  def puts(*a)
    $stdout.puts(*a)
    nil
  end
  module_function :puts

  # For each object given, prints obj.inspect followed by the
  # system record separator to standard output (thus, separator
  # cannot be overridden.) Prints nothing if no objects given.
  def p(*a)
    return nil if a.empty?
    a.each { |obj| $stdout.puts obj.inspect }
    $stdout.flush
    nil
  end
  module_function :p

  # Object oriented version of Kernel.p
  def display(port=$>)
    port.write self
  end

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

  def srand(seed=undefined)
    if seed.equal? undefined
      seed = Rubinius::Randomizer.instance.generate_seed
    end

    unless seed.respond_to?(:to_int)
      raise TypeError, "can't convert #{seed.class} into Integer"
    end

    Rubinius::Randomizer.instance.swap_seed seed.to_int
  end
  module_function :srand

  def rand(limit=0)
    limit = Integer(limit).abs

    case limit
    when 0
      Rubinius::Randomizer.instance.random_float
    when Integer
      Rubinius::Randomizer.instance.random_integer(limit - 1)
    else
      raise TypeError, "Integer() returned a non-integer"
    end
  end
  module_function :rand

  def block_given?
    return Rubinius::VariableScope.of_sender.block != nil
  end
  module_function :block_given?

  alias_method :iterator?, :block_given?
  module_function :iterator?

  def lambda(&prc)
    raise ArgumentError, "block required" unless prc
    prc.lambda_style!
    return prc
  end
  alias_method :proc, :lambda
  module_function :lambda
  module_function :proc

  def caller(start=1, exclude_kernel=true)
    # The + 1 is to skip this frame
    Rubinius.mri_backtrace(start + 1).map do |tup|
      cm = tup[0]
      line = tup[1]
      is_block = tup[2]
      name = tup[3]

      "#{cm.active_path}:#{line}:in `#{name}'"
    end
  end
  module_function :caller

  def global_variables
    Rubinius.convert_to_names Rubinius::Globals.variables
  end
  module_function :global_variables

  def loop
    raise LocalJumpError, "no block given" unless block_given?

    begin
      while true
        yield
      end
    rescue StopIteration
    end
  end
  module_function :loop

  #
  # Sleeps the current thread for +duration+ seconds.
  #
  def sleep(duration=undefined)
    if duration.equal? undefined
      duration = nil
    elsif !duration.kind_of?(Numeric)
      raise TypeError, 'time interval must be a numeric value'
    end

    start = Process.time
    chan = Rubinius::Channel.new
    chan.receive_timeout duration

    return Process.time - start
  end
  module_function :sleep

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

  def tap
    yield self
    self
  end

  alias_method :object_id, :__id__

  def id
    Kernel.warn "Object#id IS deprecated; use Object#object_id OR ELSE."
    __id__
  end

  def type
    Kernel.warn "Object#type IS fully deprecated; use Object#class OR ELSE."
    self.class
  end

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

  def extend(*modules)
    Ruby.check_frozen

    modules.reverse_each do |mod|
      mod.__send__(:extend_object, self)
      mod.__send__(:extended, self)
    end
    self
  end

  alias_method :__extend__, :extend

  def inspect
    # The protocol here seems odd, but it's to match MRI.

    ivars = instance_variables

    # If this object has no ivars, then we call to_s and return that.
    # NOTE MRI has an additional check for when to call to_s. It also does:
    # "if (TYPE(obj) == T_OBJECT ..." ie, for all builtin types, don't run
    # this code. I'm going to ignore that for now, since the builtin types
    # generally have their own inspect anyway, and if for some reason things
    # get here, thats ok.
    return to_s if ivars.empty?

    prefix = "#<#{self.class}:0x#{self.__id__.to_s(16)}"

    # Otherwise, if it's already been inspected, return the ...
    return "#{prefix} ...>" if Thread.guarding? self

    # Otherwise, gather the ivars and show them.
    parts = []

    Thread.recursion_guard self do
      ivars.each do |var|
        parts << "#{var}=#{instance_variable_get(var).inspect}"
      end
    end

    if parts.empty?
      str = "#{prefix}>"
    else
      str = "#{prefix} #{parts.join(' ')}>"
    end

    str.taint if tainted?

    return str
  end

  ##
  # Returns true if this object is an instance of the given class, otherwise
  # false. Raises a TypeError if a non-Class object given.
  #
  # Module objects can also be given for MRI compatibility but the result is
  # always false.

  def instance_of?(cls)
    Ruby.primitive :object_instance_of

    if cls.class != Class and cls.class != Module
      # We can obviously compare against Modules but result is always false
      raise TypeError, "instance_of? requires a Class argument"
    end

    self.class == cls
  end

  alias_method :__instance_of__, :instance_of?

  def instance_variable_get(sym)
    Ruby.primitive :object_get_ivar

    sym = Rubinius.instance_variable_validate sym
    instance_variable_get sym
  end

  alias_method :__instance_variable_get__, :instance_variable_get

  def instance_variable_set(sym, value)
    Ruby.primitive :object_set_ivar

    sym = Rubinius.instance_variable_validate sym
    instance_variable_set sym, value
  end

  alias_method :__instance_variable_set__, :instance_variable_set

  def remove_instance_variable(sym)
    Ruby.primitive :object_del_ivar

    # If it's already a symbol, then we're here because it doesn't exist.
    if sym.kind_of? Symbol
      raise NameError, "instance variable '#{sym}' not defined"
    end

    # Otherwise because sym isn't a symbol, coerce it and try again.
    remove_instance_variable Rubinius.instance_variable_validate(sym)
  end
  private :remove_instance_variable

  def all_instance_variables
    Ruby.primitive :object_ivar_names

    raise PrimitiveFailure, "Object#instance_variables failed"
  end
  private :all_instance_variables

  def instance_variables
    ary = []
    all_instance_variables.each do |sym|
      ary << sym.to_s if sym.is_ivar?
    end

    return ary
  end

  alias_method :__instance_variables__, :instance_variables

  def instance_variable_defined?(name)
    Ruby.primitive :object_ivar_defined

    instance_variable_defined? Rubinius.instance_variable_validate(name)
  end

  # Both of these are for defined? when used inside a proxy obj that
  # may undef the regular method. The compiler generates __ calls.
  alias_method :__instance_variable_defined_p__, :instance_variable_defined?
  alias_method :__respond_to_p__, :respond_to?

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
    name = Type.coerce_to_symbol name
    cm = Rubinius.find_method(self, name)

    if cm
      return Method.new(self, cm[1], cm[0], name)
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end

  def nil?
    false
  end

  alias_method :__nil__, :nil?

  def methods(all=true)
    methods = singleton_methods(all)

    if all
      # We have to special case these because unlike true, false, nil,
      # .object_metaclass raises a TypeError.
      case self
      when Fixnum, Symbol
        methods |= self.class.instance_methods(true)
      else
        methods |= Rubinius.object_metaclass(self).instance_methods(true)
      end
    end

    return methods if kind_of?(ImmediateValue)

    undefs = []
    Rubinius.object_metaclass(self).method_table.filter_entries do |entry|
      undefs << entry.name.to_s if entry.visibility == :undef
    end

    return methods - undefs
  end

  def private_methods(all=true)
    private_singleton_methods() | self.class.private_instance_methods()
  end

  def private_singleton_methods
    mc = Rubinius.object_metaclass self
    methods = mc.method_table.private_names

    m = mc

    while m = m.direct_superclass
      break unless m.kind_of?(Rubinius::IncludedModule) || m.__metaclass_object__

      methods.concat m.method_table.private_names
    end

    Rubinius.convert_to_names methods
  end
  private :private_singleton_methods

  def protected_methods(all=true)
    protected_singleton_methods() | self.class.protected_instance_methods(all)
  end

  def protected_singleton_methods
    mc = Rubinius.object_metaclass self
    methods = mc.method_table.protected_names

    m = mc

    while m = m.direct_superclass
      break unless m.kind_of?(Rubinius::IncludedModule) || m.__metaclass_object__

      methods.concat m.method_table.protected_names
    end

    Rubinius.convert_to_names methods
  end
  private :protected_singleton_methods

  def public_methods(all=true)
    singleton_methods(all) | self.class.public_instance_methods(all)
  end

  def singleton_methods(all=true)
    mc = Rubinius.object_metaclass self
    mt = mc.method_table
    methods = mt.public_names + mt.protected_names

    if all
      m = mc

      while m = m.direct_superclass
        break unless m.kind_of?(Rubinius::IncludedModule) || m.__metaclass_object__

        mt = m.method_table
        methods.concat mt.public_names
        methods.concat mt.protected_names
      end
    end

    Rubinius.convert_to_names methods.uniq
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
    str = "#<#{self.class}:0x#{self.__id__.to_s(16)}>"
    str.taint if tainted?
    return str
  end

  ##
  # Loads the given file as executable code and returns true. If
  # the file cannot be found, cannot be compiled or some other
  # error occurs, LoadError is raised with an explanation.
  #
  # Unlike #require, the file extension if any must be present but
  # is not restricted to .rb, .rbc or .<platform shared lib ext>.
  # Any other extensions (or no extension) are assumed to be plain
  # Ruby files. The only exceptions to this rule are:
  #
  # 1.  if given a .rb or no/any-extensioned file and there is a
  #     compiled version of the same file that is not older than
  #     the source file (based on File.mtime), the compiled one
  #     is loaded directly to avoid the compilation overhead.
  # 2.  if a .rb file is given but it does not exist, the system
  #     will try to load the corresponding .rbc instead (to allow
  #     distributing just .rbc files.)
  #
  # If the path given starts with ./, ../, ~/ or /, it is treated
  # as a "qualified" file and will be loaded directly (after path
  # expansion) instead of matching against $LOAD_PATH. The relative
  # paths use Dir.pwd.
  #
  # If the filename is plain (unqualified) then it is sequentially
  # prefixed with each path in $LOAD_PATH ($:) to locate the file,
  # using the first one that exists. If none of the resulting paths
  # exist, LoadError is raised. Unqualified names may contain path
  # elements so directories are valid targets and can be used with
  # $LOAD_PATH.
  #
  # A few extra options are supported. If the second parameter is
  # true, then the module is wrapped inside an anonymous module for
  # loading to avoid polluting the namespace. This is actually a
  # shorthand for passing in :wrap => true-ish in the second arg
  # which may be an option Hash.
  #
  # If :recompile in option Hash is true-ish then the file in
  # question is recompiled each time. If the source file is not
  # present when recompiling is requested, a LoadError is raised.
  #
  # TODO: Support non-UNIX paths.
  #
  # TODO: The anonymous module wrapping is not implemented at all.
  #
  def load(name, wrap=false)
    Rubinius::CodeLoader.new(name).load(wrap)

    # HACK we use __send__ here so that the method inliner
    # doesn't accidentally inline a script body into here!
    MAIN.__send__ :__script__

    Rubinius::CodeLoader.loaded_hook.trigger!(name)

    return true
  end
  module_function :load

  # Attempt to load the given file, returning true if successful.
  # If the file has already been successfully loaded and exists
  # in $LOADED_FEATURES, it will not be re-evaluated and false
  # is returned instead. If the filename cannot be resolved,
  # a LoadError is raised.
  #
  # The file can have one of the following extensions:
  #
  # [.rb]                   Plain Ruby source file.
  # [.rbc]                  Compiled Ruby source file.
  # [.o, .so, .dylib, .dll] Shared library (platform-specific.)
  # [<none>]                Filename without extension.
  #
  # (.rba files should be loaded using CodeArchive.load_everything.)
  #
  # If the file does not have an extension, #require attempts to
  # match it using .rb, .rbc and .<shared extension> as extensions,
  # in that order, instead. If foo.rb does not exist but foo.rbc
  # does, the latter will be loaded even if called with foo.rb.
  #
  # If the path given starts with ./, ../, ~/ or /, it is treated
  # as a "qualified" file and will be loaded directly (after path
  # expansion) instead of matching against $LOAD_PATH. The relative
  # paths use Dir.pwd.
  #
  # If the filename is plain (unqualified) then it is sequentially
  # prefixed with each path in $LOAD_PATH ($:) to locate the file,
  # using the first one that exists. If none of the resulting paths
  # exist, LoadError is raised. Unqualified names may contain path
  # elements so directories are valid targets and can be used with
  # $LOAD_PATH.
  #
  # TODO: Support non-UNIX paths.
  #
  # TODO: See if we can safely use 1.9 rules with $LOADED_FEATURES,
  #       i.e. expand paths into it. This should be possible if it
  #       is completely transparent to the user in all normal cases.
  #
  # Each successfully loaded file is added to $LOADED_FEATURES
  # ($"), using the original unexpanded filename (with the
  # exception that the file extension is added.)
  #
  def require(name)
    Rubinius::CodeLoader.require name
  end
  module_function :require

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
    Kernel.ensure_last_read_string
    $_ = $_.chomp(string)
  end
  module_function :chomp

  def chomp!(string=$/)
    Kernel.ensure_last_read_string
    $_.chomp!(string)
  end
  module_function :chomp!

  def chop(string=$/)
    Kernel.ensure_last_read_string
    $_ = $_.chop(string)
  end
  module_function :chop

  def chop!(string=$/)
    Kernel.ensure_last_read_string
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
    Kernel.ensure_last_read_string
    $_ = $_.gsub(pattern, rep, &block)
  end
  module_function :gsub

  def gsub!(pattern, rep=nil, &block)
    Kernel.ensure_last_read_string
    $_.gsub!(pattern, rep, &block)
  end
  module_function :gsub!

  def sub(pattern, rep=nil, &block)
    Kernel.ensure_last_read_string
    $_ = $_.sub(pattern, rep, &block)
  end
  module_function :sub

  def sub!(pattern, rep=nil, &block)
    Kernel.ensure_last_read_string
    $_.sub!(pattern, rep, &block)
  end
  module_function :sub!

  def scan(pattern, &block)
    Kernel.ensure_last_read_string
    $_.scan(pattern, &block)
  end
  module_function :scan

  def select(*args)
    IO.select(*args)
  end
  module_function :select

  def split(*args)
    Kernel.ensure_last_read_string
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
  #--
  # *args is used to simulate optional prepended argument like MRI

  def initialize(first=nil, *args)
    if first.kind_of?(Fixnum)
      status = first
      super(*args)
    else
      status = Process::EXIT_SUCCESS
      super
    end

    @status = status
  end

end


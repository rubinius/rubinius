# -*- encoding: us-ascii -*-

module Kernel
  def method(name)
    name = Rubinius::Type.coerce_to_symbol name
    code = Rubinius.find_method(self, name)

    if code
      Method.new(self, code[1], code[0], name)
    elsif respond_to_missing?(name, true)
      Method.new(self, self.class, Rubinius::MissingMethod.new(self,  name), name)
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end

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

  alias_method :__callee__, :__method__
  module_function :__callee__

  def define_singleton_method(*args, &block)
    singleton_class.send(:define_method, *args, &block)
  end

  def loop
    return to_enum(:loop) unless block_given?

    begin
      while true
        yield
      end
    rescue StopIteration
    end
  end
  module_function :loop

  def rand(limit=0)
    if limit == 0
      return Thread.current.randomizer.random_float
    end

    if limit.kind_of?(Range)
      return Thread.current.randomizer.random_range(limit)
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

  def to_enum(method=:each, *args)
    Enumerator.new(self, method, *args)
  end
  alias_method :enum_for, :to_enum

  # Send message to object with given arguments.
  #
  # Ignores visibility of method, and may therefore be used to invoke
  # protected or private methods.
  #
  # As denoted by the double-underscore, this method must not be removed or
  # redefined by user code.
  #
  # In 1.8, :send is an alias to :__send__ because both methods are defined on
  # Kernel. But in 1.9, :__send__ is defined on BasicObject.
  #
  def send(message, *args)
    Rubinius.primitive :object_send
    raise PrimitiveFailure, "Kernel#send primitive failed"
  end

  def public_send(message, *args)
    Rubinius.primitive :object_public_send
    raise PrimitiveFailure, "Kernel#public_send primitive failed"
  end

  # In 1.8, :object_id is an alias to :__id__ because both methods are defined
  # on Kernel. But in 1.9, :__id__ is defined on BasicObject.
  #
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

  # Attempt to load the given file, returning true if successful. Works just
  # like Kernel#require, except that it searches relative to the current
  # directory.
  #
  def require_relative(name)
    scope = Rubinius::ConstantScope.of_sender
    Rubinius::CodeLoader.require_relative(name, scope)
  end
  module_function :require_relative

  def lambda
    env = nil

    Rubinius.asm do
      push_block
      # assign a pushed block to the above local variable "env"
      # Note that "env" is indexed at 0.
      set_local 0
    end

    raise ArgumentError, "block required" unless env

    prc = Proc.__from_block__(env)

    # Make a proc lambda only when passed an actual block (ie, not using the
    # "&block" notation), otherwise don't modify it at all.
    prc.lambda_style! if env.is_a?(Rubinius::BlockEnvironment)

    return prc
  end

  module_function :lambda

  def proc(&prc)
    raise ArgumentError, "block required" unless prc
    return prc
  end
  module_function :proc

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

  def =~(other)
    nil
  end

  def Float(obj)
    raise TypeError, "can't convert nil into Float" if obj.nil?

    case obj
    when Float
      obj
    when String
      Rubinius::Type.coerce_to_float(obj, true, false)
    else
      Rubinius::Type.coerce_to(obj, Float, :to_f)
    end
  end
  module_function :Float

  def Complex(*args)
    Rubinius.privately do
      Complex.convert *args
    end
  end
  module_function :Complex

  def Rational(a, b = 1)
    Rubinius.privately do
      Rational.convert a, b
    end
  end
  module_function :Rational

  # obj <=> other -> 0 or nil
  def <=>(other)
    self == other ? 0 : nil
  end

  def initialize_dup(other)
    initialize_copy(other)
  end

  def initialize_clone(other)
    initialize_copy(other)
  end
end

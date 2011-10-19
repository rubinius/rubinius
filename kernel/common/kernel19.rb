module Kernel
  alias_method :__callee__, :__method__

  def define_singleton_method(*args, &block)
    class << self
      self
    end.send(:define_method, *args, &block)
  end

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

    # MRI checks for Fixnum explicitly and raises ArgumentError
    # instead of TypeError. Seems silly, so we don't bother.
    #
    case message
    when String
      message = Rubinius::Type.coerce_to message, Symbol, :to_sym
    when Symbol
      # nothing!
    else
      raise TypeError, "#{message.inspect} is not a symbol"
    end

    __send__ message, *args
  end

  def proc(&prc)
    raise ArgumentError, "block required" unless prc
    return prc
  end

  module_function :proc

  # Attempt to load the given file, returning true if successful. Works just
  # like Kernel#require, except that it searches relative to the current
  # directory.
  #
  def require_relative(name)
    scope = Rubinius::StaticScope.of_sender
    Rubinius::CodeLoader.require_relative(name, scope)
  end
  module_function :require_relative

  def String(obj)
    return obj if obj.kind_of? String

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
end

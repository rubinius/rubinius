module Kernel
  alias_method :send, :__send__

  def to_a
    if self.kind_of? Array
      self
    else
      [self]
    end
  end

  def Integer(obj)
    case obj
    when Integer
      obj
    when Float
      if obj.nan? or obj.infinite?
        raise FloatDomainError, "unable to coerce #{obj} to Integer"
      else
        obj.to_int
      end
    when String
      if obj.empty?
        raise ArgumentError, "invalid value for Integer: (empty string)"
      else
        obj.to_inum(0, true)
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

  alias_method :proc, :lambda
  module_function :proc

  def String(obj)
    return obj if obj.kind_of? String

    unless obj.respond_to? :to_s
      raise TypeError, "Unable to convert to a String"
    end

    str = obj.to_s
    unless str.kind_of? String
      raise TypeError, "#to_s did not return a String"
    end

    return str
  end
  module_function :String
end

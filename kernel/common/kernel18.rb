module Kernel
  alias_method :send, :__send__

  def to_a
    if self.kind_of? Array
      self
    else
      [self]
    end
  end

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
  
  def Array(obj)
    ary = Rubinius::Type.check_convert_type obj, Array, :to_ary

    return ary if ary

    if obj.respond_to? :to_a
      Rubinius::Type.coerce_to(obj, Array, :to_a)
    else
      [obj]
    end
  end
  module_function :Array
  
  def Float(obj)
    raise TypeError, "can't convert nil into Float" if obj.nil?

    case obj
    when Float
      obj
    when String
      valid_re = /^\s*[+-]?((\d+_?)*\d+(\.(\d+_?)*\d+)?|\.(\d+_?)*\d+)(\s*|([eE][+-]?(\d+_?)*\d+)\s*)$/

      m = valid_re.match(obj)

      if !m or !m.pre_match.empty? or !m.post_match.empty?
        raise ArgumentError, "invalid value for Float(): #{obj.inspect}"
      end
      obj.convert_float
    else
      coerced_value = Rubinius::Type.coerce_to(obj, Float, :to_f)
      if coerced_value.nan?
        raise ArgumentError, "invalid value for Float(): #{coerced_value.inspect}"
      end
      coerced_value
    end
  end
  module_function :Float
  
end

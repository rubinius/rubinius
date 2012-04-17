# -*- encoding: us-ascii -*-

module Kernel
  def method(name)
    name = Rubinius::Type.coerce_to_symbol name
    cm = Rubinius.find_method(self, name)

    if cm
      Method.new(self, cm[1], cm[0], name)
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end

  alias_method :send, :__send__
  alias_method :object_id, :__id__

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

  def rand(limit=0)
    unless limit == 0
      limit = Integer(limit).abs
    end

    case limit
    when 0
      Thread.current.randomizer.random_float
    when Integer
      Thread.current.randomizer.random_integer(limit - 1)
    else
      raise TypeError, "Integer() returned a non-integer"
    end
  end
  module_function :rand

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

  def open(path, *rest, &block)
    path = StringValue(path)

    if path.kind_of? String and path.prefix? '|'
      return IO.popen(path[1..-1], *rest, &block)
    end

    File.open(path, *rest, &block)
  end
  module_function :open
  
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
      Rubinius::Type.coerce_to_float(obj, true, false)
    else
      coerced_value = Rubinius::Type.coerce_to(obj, Float, :to_f)
      if coerced_value.nan?
        raise ArgumentError, "invalid value for Float(): #{coerced_value.inspect}"
      end
      coerced_value
    end
  end
  module_function :Float

  def id
    Kernel.warn "Object#id IS deprecated; use Object#object_id OR ELSE."
    __id__
  end

  def type
    Kernel.warn "Object#type IS fully deprecated; use Object#class OR ELSE."
    self.class
  end

  def method(name)
    name = Rubinius::Type.coerce_to_symbol name
    cm = Rubinius.find_method(self, name)

    if cm
      return Method.new(self, cm[1], cm[0], name)
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end
end

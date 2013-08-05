# -*- encoding: us-ascii -*-

module Kernel
  alias_method :__instance_variables__, :instance_variables
  
  def __all_instance_variables__
    Rubinius.primitive :object_ivar_names

    raise PrimitiveFailure, "Object#instance_variables failed"
  end
  private :__all_instance_variables__

  def method(name)
    name = Rubinius::Type.coerce_to_symbol name
    code = Rubinius.find_method(self, name)

    if code
      Method.new(self, code[1], code[0], name)
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
    if limit == 0
      return Thread.current.randomizer.random_float
    end

    limit = Integer(limit).abs

    if limit == 0
      Thread.current.randomizer.random_float
    else
      Thread.current.randomizer.random_integer(limit - 1)
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

  def lambda(&prc)
    raise ArgumentError, "block required" unless prc
    prc.lambda_style!
    return prc
  end

  module_function :lambda
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

    if obj.respond_to? :to_a, true
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
    code = Rubinius.find_method(self, name)

    if code
      return Method.new(self, code[1], code[0], name)
    else
      raise NameError, "undefined method `#{name}' for #{self.inspect}"
    end
  end

  def gsub(pattern, rep=nil, &block)
    target = $_
    raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
    $_ = target.gsub(pattern, rep, &block)
  end
  module_function :gsub

  def gsub!(pattern, rep=nil, &block)
    target = $_
    raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
    target.gsub!(pattern, rep, &block)
  end
  module_function :gsub!

  def sub(pattern, rep=nil, &block)
    target = $_
    raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
    $_ = target.sub(pattern, rep, &block)
  end
  module_function :sub

  def sub!(pattern, rep=nil, &block)
    target = $_
    raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
    target.sub!(pattern, rep, &block)
  end
  module_function :sub!

  def scan(pattern, &block)
    target = $_
    raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
    target.scan(pattern, &block)
  end
  module_function :scan

  def split(*args)
    target = $_
    raise TypeError, "$_ must be a String, but is #{target.inspect}" unless target.kind_of? String
    target.split(*args)
  end
  module_function :split

  def chomp(string=$/)
    raise TypeError, "$_ must be a String" unless $_.kind_of? String
    $_ = $_.chomp(string)
  end
  module_function :chomp

  def chomp!(string=$/)
    raise TypeError, "$_ must be a String" unless $_.kind_of? String
    $_.chomp!(string)
  end
  module_function :chomp!

  def chop
    raise TypeError, "$_ must be a String" unless $_.kind_of? String
    $_ = $_.chop
  end
  module_function :chop

  def chop!
    raise TypeError, "$_ must be a String" unless $_.kind_of? String
    $_.chop!
  end
  module_function :chop!
end

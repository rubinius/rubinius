class Method
  def initialize(recv, mod, cm)
    @receiver = recv
    @method = cm
    @module = mod
  end
  
  def inspect
    "#<#{self.class} #{@receiver.class}(#{@module})##{@method.name}>"
  end
  
  def call(*args, &prc)
    @method.activate(@receiver, args, &prc)
  end

  def unbind
    UnboundMethod.new(@module, @method)
  end

  def arity
    @method.arity
  end
  
  def location
    "#{@method.file}, near line #{@method.first_line}"
  end
  
  def compiled_method
    @method
  end
  
  # Duck typing ftw!
  def to_proc
    self
  end

end

class UnboundMethod
  def initialize(mod, cm)
    @method = cm
    @module = mod
  end
  
  def inspect
    "#<#{self.class} #{@module}##{@method.name}>"
  end

  def bind(receiver)
    Method.new(receiver, @module, @method)
  end

  def arity
    @method.arity
  end

  def call_on_instance(obj, *args)
    bind(obj).call *args
  end
end

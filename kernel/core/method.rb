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
    @method.activate(@receiver, @module, args, &prc)
  end

  def unbind
    UnboundMethod.new(@module, @method)
  end

  def arity
    @method.required
  end
  
  def location
    "#{@method.file}, near line #{@method.first_line}"
  end
  
  def compiled_method
    @method
  end
  
  def to_proc
    env = Method::AsBlockEnvironment.new self
    Proc.from_environment(env)
  end
end

class Method::AsBlockEnvironment < BlockEnvironment
  def initialize(method)
    @method = method
  end
  def method; @method.compiled_method; end
  def file; method.file; end
  def line; method.first_line; end
  def redirect_to(obj)
    @method = @method.unbind.bind(obj)
  end
  def call(*args); @method.call(*args); end
  def call_on_instance(obj, *args)
    redirect_to(obj).call(*args)
  end
  def arity; @method.arity; end
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
    @method.required
  end

  def call_on_instance(obj, *args)
    bind(obj).call *args
  end
end

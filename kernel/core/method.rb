# depends on: class.rb

class Method
  attr_reader :module
  attr_reader :receiver
  attr_reader :method
  
  def initialize(recv, mod, cm)
    @receiver = recv
    @method = cm
    @module = mod
  end
  
  def inspect
    "#<#{self.class} #{@receiver.class}(#{@module})##{@method.name}>"
  end

  alias_method :to_s, :inspect

  def call(*args, &prc)
    @method.activate(@receiver, @module, args, &prc)
  end

  alias_method :[], :call

  def unbind
    UnboundMethod.new(@module, @method, @receiver.class)
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

  # Method objects are equal if they have the
  # same body and are bound to the same object.
  def ==(other)
    if other.kind_of? Method
      return true if other.receiver.equal?(@receiver) && other.method == @method
    end

    false
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
  def initialize(mod, cm, orig_rcv = nil)
    @method = cm
    @module = mod
    @orig_receiver = orig_rcv
  end
  
  def inspect
    "#<#{self.class} #{@module}##{@method.name}>"
  end

  def bind(receiver)
    raise TypeError if @orig_receiver && !receiver.class.ancestors.include?(@orig_receiver)
    Method.new(receiver, @module, @method)
  end

  def arity
    @method.required
  end

  def compiled_method
    @method
  end
  
  def call_on_instance(obj, *args)
    bind(obj).call *args
  end
end

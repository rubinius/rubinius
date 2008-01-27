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

# UnboundMethods are similar to Method objects except that they
# are not connected to any particular object. They cannot be used
# standalone for this reason, and must be bound to an object first.
# The object must be kind_of? the Module in which this method was
# originally defined.
#
# UnboundMethods can be created in two ways: first, any existing
# Method object can be sent #unbind to detach it from its current
# object and return an UnboundMethod instead. Secondly, they can
# be directly created by calling Module#instance_method with the
# desired method's name.
#
# The UnboundMethod is a copy of the method as it existed at the
# time of creation. Any subsequent changes to the original will
# not affect any existing UnboundMethods.
class UnboundMethod
  # Accepts and stores the Module where the method is defined
  # in as well as the CompiledMethod itself. Class of the object
  # the method was extracted from can be given but will not be
  # stored. This is always used internally only.
  def initialize(mod, compiled_method, original_receiver_class = nil)
    @defined_in, @compiled_method = mod, compiled_method
  end

  attr_reader :compiled_method
  attr_reader :defined_in
  protected   :compiled_method
  protected   :defined_in

  # Instance methods

  # UnboundMethod objects are equal if and only if they refer to
  # the same method. One may be an alias for the other or both
  # for a common one. Both must have been extracted from the same
  # class or subclass. Two from different subclasses will not be
  # considered equal.
  def ==(other)
    unless other.kind_of? UnboundMethod and
           @defined_in == other.defined_in and
           @compiled_method == other.compiled_method
      return false
    end

    true
  end

  # See Method#arity.
  def arity()
    @compiled_method.required
  end

  # Creates a new Method object by attaching this compiled_method to the
  # supplied receiver. The receiver must be kind_of? original
  # Module object extracted from.
  def bind(receiver)
    unless receiver.kind_of? @defined_in
      raise TypeError, "Must be bound to an object of kind #{@defined_in}"
    end
    Method.new receiver, @defined_in, @compiled_method
  end

  # Convenience method for #binding to the given receiver object and
  # calling it with the optionally supplied arguments.
  def call_on_instance(obj, *args)
    bind(obj).call *args
  end

  # Returns a String representation thet indicates our class
  # as well as the compiled_method name and the Module the compiled_method was
  # extracted from.
  def inspect()
    "#<#{self.class} #{@defined_in}##{@compiled_method.name}>"
  end

  alias_method :to_s, :inspect

end

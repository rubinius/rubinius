# -*- encoding: us-ascii -*-

class Method
  def name
    @name
  end

  ##
  # Method objects are equal if they have the same body and are bound to the
  # same object.

  def ==(other)
    other.class == Method &&
      @receiver.equal?(other.receiver) &&
      (@executable == other.executable ||
       Rubinius::MethodEquality.method_equal_to_delegated_method_receiver?(self, other))
  end

  alias_method :eql?, :==
end

class UnboundMethod
  def name
    @name
  end

  ##
  # Convenience method for #binding to the given receiver object and calling
  # it with the optionally supplied arguments.

  def call_on_instance(obj, *args, &block)
    @executable.invoke(@name, @defined_in, obj, args, block)
  end
end

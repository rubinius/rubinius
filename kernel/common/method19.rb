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

end


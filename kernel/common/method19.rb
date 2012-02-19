# -*- encoding: us-ascii -*-

class Method
  def name
    @name
  end

  ##
  # Method objects are equal if they have the same body and are bound to the
  # same object.

  def ==(other)
    other.class == Method and
      @receiver.equal?(other.receiver) and
      (@executable == other.executable or
       equal_to_delegated_method_receiver?(other))
  end

  alias_method :eql?, :==

  protected

  def equal_to_delegated_method_receiver?(other)
    self_receiver  = delegated_receiver_for(self)  || unbind
    other_receiver = delegated_receiver_for(other) || other.unbind
    self_or_other_is_delegated?(other) && (self_receiver == other_receiver)
  end

  def self_or_other_is_delegated?(other)
    (@executable.kind_of?(Rubinius::CompiledMethod) && other.executable.kind_of?(Rubinius::DelegatedMethod)) ||
    (@executable.kind_of?(Rubinius::DelegatedMethod) && other.executable.kind_of?(Rubinius::CompiledMethod))
  end

  def delegated_receiver_for(meth)
    meth.executable.receiver if meth.executable.kind_of?(Rubinius::DelegatedMethod)
  end
end

class UnboundMethod

  def name
    @name
  end

end


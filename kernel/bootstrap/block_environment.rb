class BlockEnvironment
  def self.allocate
    Ruby.primitive :blockenvironment_allocate
    raise PrimitiveFailure, "BlockEnvironment.allocate primitive failed"
  end

  def call(*args)
    Ruby.primitive :block_call
    raise PrimitiveFailure, "BlockEnvironment#call primitive failed"
  end
end

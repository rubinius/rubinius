class Proc
  class FromBlock
    def self.allocate
      Ruby.primitive :block_wrapper_allocate
      raise PrimitiveFailure, "BlockWrapper#allocate failed"
    end

    def call(*args)
      Ruby.primitive :block_wrapper_call
      raise PrimitiveFailure, "BlockWrapper#call failed"
    end
  end
end

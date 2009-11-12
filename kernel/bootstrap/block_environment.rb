module Rubinius
  class BlockEnvironment
    def self.allocate
      Ruby.primitive :blockenvironment_allocate
      raise PrimitiveFailure, "BlockEnvironment.allocate primitive failed"
    end

    def call(*args)
      Ruby.primitive :block_call
      raise PrimitiveFailure, "BlockEnvironment#call primitive failed"
    end

    def call_under(recv, static_scope, *args)
      Ruby.primitive :block_call_under
      raise PrimitiveFailure, "BlockEnvironment#call_under primitive failed"
    end

    class AsMethod < Executable
      def self.new(block_env)
        Ruby.primitive :block_as_method_create
        raise PrimitiveFailure, "BlockEnvironment::AsMethod.new failed"
      end
    end
  end
end

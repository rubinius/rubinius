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

      def arity
        @block_env.method.arity
      end

      def local_names
        @block_env.method.local_names
      end

      def required_args
        @block_env.method.required_args
      end

      def total_args
        @block_env.method.total_args
      end

      def splat
        @block_env.method.splat
      end
    end
  end
end

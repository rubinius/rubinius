module Rubinius
  class BlockEnvironment
    def self.allocate
      Rubinius.primitive :blockenvironment_allocate
      raise PrimitiveFailure, "BlockEnvironment.allocate primitive failed"
    end

    def call(*args)
      Rubinius.primitive :block_call
      raise PrimitiveFailure, "BlockEnvironment#call primitive failed"
    end

    def call_under(recv, static_scope, *args)
      Rubinius.primitive :block_call_under
      raise PrimitiveFailure, "BlockEnvironment#call_under primitive failed"
    end

    def self.of_sender
      Rubinius.primitive :block_env_of_sender
      raise PrimitiveFailure, "BlockEnvironment#of_sender primitive failed"
    end

    class AsMethod < Executable
      def self.new(block_env)
        Rubinius.primitive :block_as_method_create
        raise PrimitiveFailure, "BlockEnvironment::AsMethod.new failed"
      end

      def arity
        @block_env.code.arity
      end

      def local_names
        @block_env.code.local_names
      end

      def required_args
        @block_env.code.required_args
      end

      def total_args
        @block_env.code.total_args
      end

      def post_args
        @block_env.code.post_args
      end

      def splat
        @block_env.code.splat
      end

      def file
        @block_env.file
      end

      def defined_line
        @block_env.line
      end
    end
  end
end

module Rubinius
  module JIT
    class << self
      attr_accessor :available
      attr_accessor :enabled
      attr_accessor :properties

      alias_method :available?, :available
      alias_method :enabled?, :enabled

      def compile(object, compiled_code, block_environment=nil)
        Rubinius.invoke_primitive :jit_compile, self, object, compiled_code, block_environment
      end

      # TODO: Fix configuration
      def compile_threshold
        Rubinius.primitive :jit_compile_threshold
        raise PrimitiveFailure, "Rubinius::JIT.compile_threshold primitive failed"
      end

      def sync=(flag)
        Rubinius.primitive :jit_sync_set
        raise PrimitiveFailure, "Rubinius::JIT.sync primitive failed"
      end

      def sync
        Rubinius.primitive :jit_sync_get
        raise PrimitiveFailure, "Rubinius::JIT.sync primitive failed"
      end
    end
  end
end

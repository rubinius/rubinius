module Rubinius
  module JIT
    class << self
      attr_accessor :properties

      def compile(object, compiled_code, block_environment=nil)
        Rubinius.invoke_primitive :jit_compile, self, object, compiled_code, block_environment
      end

      def enabled?
        Rubinius.primitive :jit_enabled_p
        raise PrimitiveFailure, "Rubinius::JIT.enabled? primitive failed"
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

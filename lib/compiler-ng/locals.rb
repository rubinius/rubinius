module Rubinius
  class CompilerNG
    class LocalVariable
      attr_reader :slot

      def initialize(slot)
        @slot = slot
      end
    end

    class LocalReference
      attr_reader :slot

      def initialize(var)
        @slot = var.slot
      end

      def get_bytecode(g)
        g.push_local @slot
      end

      def set_bytecode(g)
        g.set_local @slot
      end
    end

    class NestedLocalReference
      attr_accessor :depth
      attr_reader :slot

      def initialize(var)
        @slot = var.slot
        @depth = 0
      end

      def get_bytecode(g)
        g.push_local_depth @depth, @slot
      end

      def set_bytecode(g)
        g.set_local_depth @depth, @slot
      end
    end

    class EvalLocalReference
      def get_bytcode(g)
      end

      def set_bytecode(g)
      end
    end
  end
end

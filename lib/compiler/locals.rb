module Rubinius
  class Compiler
    module LocalVariables
      def variables
        @variables ||= {}
      end

      def local_count
        variables.size
      end

      def local_names
        names = []
        eval_names = []
        variables.each_pair do |name, var|
          if var.kind_of? EvalLocalVariable
            eval_names << name
          else
            names[var.slot] = name
          end
        end
        names += eval_names
      end

      def allocate_slot
        variables.size
      end
    end

    class LocalVariable
      attr_reader :slot

      def initialize(slot)
        @slot = slot
      end

      def reference
        LocalReference.new @slot
      end

      def nested_reference
        NestedLocalReference.new @slot
      end
    end

    class NestedLocalVariable
      attr_reader :depth, :slot

      def initialize(depth, slot)
        @depth = depth
        @slot = slot
      end

      def reference
        NestedLocalReference.new @slot, @depth
      end

      alias_method :nested_reference, :reference
    end

    class EvalLocalVariable
      attr_reader :name

      def initialize(name)
        @name = name
      end

      def reference
        EvalLocalReference.new @name
      end

      alias_method :nested_reference, :reference
    end

    class LocalReference
      attr_reader :slot

      def initialize(slot)
        @slot = slot
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

      def initialize(slot, depth=0)
        @slot = slot
        @depth = depth
      end

      def get_bytecode(g)
        if @depth == 0
          g.push_local @slot
        else
          g.push_local_depth @depth, @slot
        end
      end

      def set_bytecode(g)
        if @depth == 0
          g.set_local @slot
        else
          g.set_local_depth @depth, @slot
        end
      end
    end

    class EvalLocalReference

      # Ignored, but simplifies duck-typing references
      attr_accessor :depth

      def initialize(name)
        @name = name
        @depth = 0
      end

      def get_bytecode(g)
        g.push_variables
        g.push_literal @name
        g.send :get_eval_local, 1, false
      end

      def set_bytecode(g)
        g.push_variables
        g.swap
        g.push_literal @name
        g.swap
        g.send :set_eval_local, 2, false
      end
    end
  end
end

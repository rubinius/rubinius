module Rubinius
  class CompilerNG
    # TODO: I don't really think we need you
    class Context
      def initialize(variables, method)
        @variables = variables
        @method = method
      end

      attr_accessor :variables
      attr_accessor :method

      def dynamic_locals
        @variables.dynamic_locals
      end

      def set_eval_local(name, val)
        dynamic_locals[name] = val
      end

      def eval_local_defined?(name)
        @variables.eval_local_defined?(name)
      end
    end
  end
end

module Rubinius
  class VariableScope
    attr_reader :parent
    attr_reader :block

    # CompiledMethod this scope is for.
    #
    attr_reader :method

    def self=(obj)
      @self = obj
    end

    def dynamic_locals
      @dynamic_locals ||= Rubinius::LookupTable.new
    end

    def set_eval_local(name, val)
      @parent.dynamic_locals[name] = val
    end

    def get_eval_local(name)
      scope = @parent
      while scope
        if scope.dynamic_locals.key? name
          return scope.dynamic_locals[name]
        end

        scope = scope.parent
      end

      nil
    end

    def eval_local_defined?(name)
      scope = self
      while scope
        return true if scope.dynamic_locals.key? name

        scope = scope.parent
      end

      return false
    end

    def exitted?
      @exitted
    end
  end
end

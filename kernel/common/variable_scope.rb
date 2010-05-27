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

    def eval_local_defined?(name, search=true)
      if search
        scope = self
        while scope
          return true if scope.dynamic_locals.key? name

          scope = scope.parent
        end
      else
        return true if @dynamic_locals and @dynamic_locals.key? name
      end

      return false
    end

    def local_defined?(name)
      vars = self
      while vars
        return true if vars.method.local_names.include? name
        return true if vars.eval_local_defined?(name, false)
        vars = vars.parent
      end
    end

    def exitted?
      @exitted
    end

    def super_method_defined?
      # Minor HACK __block__ as the name is ambigious. Need a better
      # way to detect that a VS is for a define_method scope.
      #
      # This code is so that if this VS is for a block, we walk up
      # to the VS for the actual method. It's complicated by the fact
      # that there may be a VS for a define_method in there, so we need
      # to stop looking if we hit one.
      if @parent and @method.name == :__block__
        vars = self

        while vars.method.name == :__block__ and nxt = vars.parent
          vars = nxt
        end

        return vars.super_method_defined?
      end

      if sup = @module.direct_superclass
        # This will probably break for define_method
        return sup.lookup_method(@method.name) != nil
      end

      return false
    end

    # Indicates if this scope is for the running of a script body
    def for_script?
      if script = @method.scope.script
        return script.compiled_method == @method
      end

      return false
    end

    def method_visibility
      if scr = method.scope.script and scr.eval?
        if scr.eval_binding
          return scr.eval_binding.variables.method_visibility
        else
          return @method_visibility
        end
      end

      return @method_visibility if @method_visibility

      # if this scope is for a script, and there is no method_visibility
      # already set, then we default to private.
      #
      # This is so that a script body has it's visibility default to private.

      return :private if for_script?

      # The default, let the caller sort it out.
      return nil
    end
  end
end

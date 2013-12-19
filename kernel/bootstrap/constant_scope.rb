module Rubinius
  class ConstantScope
    def self.of_sender
      Rubinius.primitive :constant_scope_of_sender
      raise PrimitiveFailure, "primitive failed"
    end

    def class_variable_get(sym)
      Rubinius.primitive :constant_scope_cvar_get

      raise NameError, "Invalid class variable name '#{sym}'"
    end

    def class_variable_set(sym, val)
      Rubinius.primitive :constant_scope_cvar_set

      raise NameError, "Invalid class variable name '#{sym}'"
    end

    def class_variable_defined?(sym)
      Rubinius.primitive :constant_scope_cvar_defined

      raise NameError, "Invalid class variable name '#{sym}'"
    end

    def class_variable_get_or_set(sym, val)
      Rubinius.primitive :constant_scope_cvar_get_or_set

      raise NameError, "Invalid class variable name '#{sym}'"
    end
  end

  # NOTE: Temporary alias for backwards compatibility. StaticScope will
  # eventually be deprecated in favor of ConstantScope.
  StaticScope = ConstantScope
end

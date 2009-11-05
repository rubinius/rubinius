module Rubinius
  class StaticScope
    def self.of_sender
      Ruby.primitive :static_scope_of_sender
      raise PrimitiveFailure, "primitive failed"
    end

    def class_variable_get(sym)
      Ruby.primitive :static_scope_cvar_get

      raise NameError, "Invalid class variable name '#{sym}'"
    end

    def class_variable_set(sym, val)
      Ruby.primitive :static_scope_cvar_set

      raise NameError, "Invalid class variable name '#{sym}'"
    end

    def class_variable_defined?(sym)
      Ruby.primitive :static_scope_cvar_defined

      raise NameError, "Invalid class variable name '#{sym}'"
    end

    def class_variable_get_or_set(sym, val)
      Ruby.primitive :static_scope_cvar_get_or_set

      raise NameError, "Invalid class variable name '#{sym}'"
    end
  end
end

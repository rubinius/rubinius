module Rubinius
  class VariableScope
    def self.of_sender
      Rubinius.primitive :variable_scope_of_sender
      raise PrimitiveFailure, "Rubinius::VariableScope.of_sender primitive failed"
    end

    def self.current
      Rubinius.primitive :variable_scope_current
      raise PrimitiveFailure, "Rubinius::VariableScope.current primitive failed"
    end

    def self.synthesize(method, module_, parent, self_, block, locals)
      Rubinius.primitive :variable_scope_synthesize
      raise PrimitiveFailure, "Rubinius::VariableScope.synthesize primitive failed"
    end

    def set_local(slot, obj)
      Rubinius.primitive :variable_scope_set_local
      raise PrimitiveFailure, "Rubinius::VariableScope#set_local primitive failed"
    end

    def locals
      Rubinius.primitive :variable_scope_locals
      raise PrimitiveFailure, "Rubinius::VariableScope#locals primitive failed"
    end

    def top_level_visibility?
      Rubinius.primitive :variable_scope_top_level_visibility
      raise PrimitiveFailure, "Rubinius::VariableScope#top_level_visibility primitive failed"
    end

    def script?
      Rubinius.primitive :variable_scope_script
      raise PrimitiveFailure, "Rubinius::VariableScope#script primitive failed"
    end

    def locked?
      Rubinius.primitive :variable_scope_locked
      raise PrimitiveFailure, "Rubinius::VariableScope#locked primitive failed"
    end

    def locked!
      Rubinius.primitive :variable_scope_set_locked
      raise PrimitiveFailure, "Rubinius::VariableScope#set_locked primitive failed"
    end

    # To handle Module#private, protected
    attr_accessor :method_visibility
  end
end

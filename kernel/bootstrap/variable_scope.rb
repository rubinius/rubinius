module Rubinius
  class VariableScope
    def self.of_sender
      Rubinius.primitive :variable_scope_of_sender
      raise PrimitiveFailure, "Unable to get VariableScope of sender"
    end

    def self.current
      Rubinius.primitive :variable_scope_current
      raise PrimitiveFailure, "Unable to get current VariableScope"
    end

    def locals
      Rubinius.primitive :variable_scope_locals
      raise PrimitiveFailure, "Unable to get VariableScope locals"
    end

    # To handle Module#private, protected
    attr_accessor :method_visibility
  end
end

module Rubinius
  class VariableScope
    def self.of_sender
      Ruby.primitive :variable_scope_of_sender
      raise PrimitiveFailure, "Unable to get VariableScope of sender"
    end

    def self.current
      Ruby.primitive :variable_scope_current
      raise PrimitiveFailure, "Unable to get current VariableScope"
    end

    def locals
      Ruby.primitive :variable_scope_locals
      raise PrimitiveFailure, "Unable to get VariableScope locals"
    end

    # To handle Module#private, protected
    attr_accessor :method_visibility
  end
end

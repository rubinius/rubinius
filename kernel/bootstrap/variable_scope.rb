# -*- encoding: us-ascii -*-

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

    def self.synthesize(method, module_, parent, self_, block, locals)
      Rubinius.primitive :variable_scope_synthesize
      raise PrimitiveFailure, "Unable to create a new VariableScope object"
    end

    def set_local(slot, obj)
      Rubinius.primitive :variable_scope_set_local
      raise PrimitiveFailure, "Unable to set VariableScope locals"
    end

    def locals
      Rubinius.primitive :variable_scope_locals
      raise PrimitiveFailure, "Unable to get VariableScope locals"
    end

    # To handle Module#private, protected
    attr_accessor :method_visibility
  end
end

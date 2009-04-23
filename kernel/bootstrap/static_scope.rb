module Rubinius
  class StaticScope
    def self.of_sender
      Ruby.primitive :static_scope_of_sender
      raise PrimitiveFailure, "primitive failed"
    end
  end
end

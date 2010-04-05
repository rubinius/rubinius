module Rubinius
  class Thunk
    def self.new(value)
      Ruby.primitive :thunk_create
      raise PrimitiveFailure, "Thunk.new failed"
    end
  end
end

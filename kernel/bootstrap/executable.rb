module Rubinius
  class Executable
    attr_accessor :serial
    attr_accessor :primitive

    def self.allocate
      Ruby.primitive :executable_allocate
      raise PrimitiveFailure, "Executable.allocate primitive failed"
    end
  end
end

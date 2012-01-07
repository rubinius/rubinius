# -*- encoding: us-ascii -*-

module Rubinius
  class Executable
    attr_accessor :serial
    attr_accessor :primitive

    def self.allocate
      Rubinius.primitive :executable_allocate
      raise PrimitiveFailure, "Executable.allocate primitive failed"
    end

    def invoke(name, mod, recv, args, block)
      Rubinius.primitive :executable_invoke
      raise PrimitiveFailure, "Executable#invoke primitive failed"
    end
  end
end

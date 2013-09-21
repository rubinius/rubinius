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

    def custom_call_site
      Rubinius.primitive :executable_set_custom_call_site
      raise PrimitiveFailure, "Executable#set_custom_call_site primitive failed"
    end
  end
end

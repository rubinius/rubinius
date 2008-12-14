module Rubinius
  class MachineMethod
    def disassemble
      Ruby.primitive :machine_method_show
      raise PrimitiveFailure, "primitive failed"
    end

    def activate
      Ruby.primitive :machine_method_activate
      raise PrimitiveFailure, "primitive failed"
    end
  end
end

module Rubinius
  class MachineMethod
    def disassemble
      Ruby.primitive :machine_method_show
      raise PrimitiveFailure, "Rubinius::MachineMethod#disassemble primitive failed"
    end

    def activate
      Ruby.primitive :machine_method_activate
      raise PrimitiveFailure, "Rubinius::MachineMethod#activate primitive failed"
    end
  end
end

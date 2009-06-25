module Rubinius
  class CompiledMethod < Executable

    def self.allocate
      Ruby.primitive :compiledmethod_allocate
      raise PrimitiveFailure, "CompiledMethod.allocate primitive failed"
    end

    def compile
      Ruby.primitive :compiledmethod_compile
      raise PrimitiveFailure, "CompiledMethod#compile primitive failed"
    end

    def make_machine_method
      Ruby.primitive :compiledmethod_make_machine_method
      raise PrimitiveFailure, "CompiledMethod#make_machine_method primitive failed"
    end

    def jit_soon
      Ruby.primitive :compiledmethod_jit_soon
      raise PrimitiveFailure, "CompiledMethod#jit_soon primitive failed"
    end

    def activate(recv, mod, args)
      Ruby.primitive :compiledmethod_activate
      raise PrimitiveFailure, "CompiledMethod#activate failed"
    end

    def self.of_sender
      Ruby.primitive :compiledmethod_of_sender
      raise PrimitiveFailure, "CompiledMethod.of_sender failed"
    end
  end
end

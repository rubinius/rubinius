module Rubinius
  class CompiledMethod < Executable

    def self.allocate
      Ruby.primitive :compiledmethod_allocate
      raise PrimitiveFailure, "CompiledMethod.allocate primitive failed"
    end

    def dup
      Ruby.primitive :compiledmethod_dup
      raise PrimitiveFailure, "CompiledMethod#dup primitive failed"
    end

    def compile
      Ruby.primitive :compiledmethod_compile
      raise PrimitiveFailure, "CompiledMethod#compile primitive failed"
    end

    def jit_now
      Ruby.primitive :compiledmethod_jit_now
      raise PrimitiveFailure, "CompiledMethod#jit_now primitive failed"
    end

    def jit_soon
      Ruby.primitive :compiledmethod_jit_soon
      raise PrimitiveFailure, "CompiledMethod#jit_soon primitive failed"
    end

    def self.of_sender
      Ruby.primitive :compiledmethod_of_sender
      raise PrimitiveFailure, "CompiledMethod.of_sender failed"
    end
  end
end

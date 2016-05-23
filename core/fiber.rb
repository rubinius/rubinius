module Rubinius
  class Fiber
    attr_reader :stack_size

    def self.new(**kw, &block)
      if block.nil?
        raise ArgumentError, "Fiber.new requires a block"
      end

      stack_size = Rubinius::Type.try_convert kw[:stack_size], Fixnum, :to_int

      Rubinius.invoke_primitive :fiber_new, stack_size, block, self
    end

    def self.current
      Rubinius.primitive :fiber_s_current
      raise PrimitiveFailure, "Rubinius::Fiber.current failed"
    end

    def self.yield(*args)
      Rubinius.primitive :fiber_s_yield
      raise PrimitiveFailure, "Rubinius::Fiber.yield failed"
    end

    def resume(*args)
      Rubinius.primitive :fiber_resume
      raise PrimitiveFailure, "Rubinius::Fiber#resume failed"
    end

    def transfer(*args)
      Rubinius.primitive :fiber_transfer
      raise PrimitiveFailure, "Rubinius::Fiber#transfer failed"
    end

    def alive?
      !@dead
    end
  end
end

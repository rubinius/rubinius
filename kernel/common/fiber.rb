module Rubinius
  class Fiber
    def self.create(size, callable)
      Ruby.primitive :fiber_new
      raise PrimitiveFailure, "fiber_new failed"
    end

    def self.new(size=0, &block)
      if block.nil?
        raise ArgumentError, "Fiber.new requires a block"
      end

      create(size, block)
    end

    def self.current
      Ruby.primitive :fiber_s_current
      raise PrimitiveFailure, "fiber_s_current failed"
    end

    def self.yield(*args)
      Ruby.primitive :fiber_s_yield
      raise PrimitiveFailure, "fiber_s_yield failed"
    end

    def resume(*args)
      Ruby.primitive :fiber_resume
      raise PrimitiveFailure, "fiber_resume failed"
    end

    def transfer(*args)
      Ruby.primitive :fiber_transfer
      raise PrimitiveFailure, "fiber_transfer failed"
    end

    def alive?
      !@dead
    end
  end
end

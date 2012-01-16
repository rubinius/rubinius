# -*- encoding: us-ascii -*-

module Rubinius
  class Fiber
    def self.create(callable)
      Rubinius.primitive :fiber_new
      raise NotImplementedError, "Fibers not supported on this platform"
    end

    def self.new(size=0, &block)
      if block.nil?
        raise ArgumentError, "Fiber.new requires a block"
      end

      create(block)
    end

    def self.current
      Rubinius.primitive :fiber_s_current
      raise PrimitiveFailure, "fiber_s_current failed"
    end

    def self.yield(*args)
      Rubinius.primitive :fiber_s_yield
      raise PrimitiveFailure, "fiber_s_yield failed"
    end

    def resume(*args)
      Rubinius.primitive :fiber_resume
      raise PrimitiveFailure, "fiber_resume failed"
    end

    def transfer(*args)
      Rubinius.primitive :fiber_transfer
      raise PrimitiveFailure, "fiber_transfer failed"
    end

    def alive?
      !@dead
    end
  end
end

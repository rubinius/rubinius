# -*- encoding: us-ascii -*-

module Rubinius
  class CompiledMethod < Executable

    attr_accessor :name

    def self.allocate
      Rubinius.primitive :compiledmethod_allocate
      raise PrimitiveFailure, "CompiledMethod.allocate primitive failed"
    end

    def dup
      Rubinius.primitive :compiledmethod_dup
      raise PrimitiveFailure, "CompiledMethod#dup primitive failed"
    end

    # Return the CompiledMethod for caller of the method that called
    # .of_sender.
    #
    # For example:
    #
    # def g
    #   f_cm = Rubinius::CompiledMethod.of_sender
    # end
    #
    # def f
    #   g
    # end
    #
    # f_cm is the CompiledMethod of f as requested by g.
    #
    def self.of_sender
      Rubinius.primitive :compiledmethod_of_sender
      raise PrimitiveFailure, "CompiledMethod.of_sender failed"
    end

    # Returns the CompiledMethod object for the currently executing Ruby
    # method. For example:
    #
    #   def m
    #     p Rubinius::CompiledMethod.current.name
    #   end
    #
    def self.current
      Rubinius.primitive :compiledmethod_current
      raise PrimitiveFailure, "CompiledMethod.current failed"
    end
  end
end

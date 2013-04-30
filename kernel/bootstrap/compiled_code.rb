# -*- encoding: us-ascii -*-

module Rubinius
  class CompiledCode < Executable

    attr_accessor :name

    def self.allocate
      Rubinius.primitive :compiledcode_allocate
      raise PrimitiveFailure, "CompiledCode.allocate primitive failed"
    end

    def dup
      Rubinius.primitive :compiledcode_dup
      raise PrimitiveFailure, "CompiledCode#dup primitive failed"
    end

    def call_sites
      Rubinius.primitive :compiledcode_call_sites
      raise PrimitiveFailure, "CompiledCode#call_sites primitive failed"
    end

    def constant_caches
      Rubinius.primitive :compiledcode_constant_caches
      raise PrimitiveFailure, "CompiledCode#constant_caches primitive failed"
    end

    # Return the CompiledCode for caller of the method that called
    # .of_sender.
    #
    # For example:
    #
    # def g
    #   f_code = Rubinius::CompiledCode.of_sender
    # end
    #
    # def f
    #   g
    # end
    #
    # f_code is the CompiledCode of f as requested by g.
    #
    def self.of_sender
      Rubinius.primitive :compiledcode_of_sender
      raise PrimitiveFailure, "CompiledCode.of_sender primitive failed"
    end

    # Returns the CompiledCode object for the currently executing Ruby
    # method. For example:
    #
    #   def m
    #     p Rubinius::CompiledCode.current.name
    #   end
    #
    def self.current
      Rubinius.primitive :compiledcode_current
      raise PrimitiveFailure, "CompiledCode.current primitive failed"
    end
  end

  # NOTE: Temporary alias for backwards compatibility. CompiledMethod is
  # deprecated. Client code should use CompiledCode.
  CompiledMethod = CompiledCode

end

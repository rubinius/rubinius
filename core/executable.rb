##
# Parent class of everything that can live in a MethodTable.
#
# When calling an Executable, first the primitive slot is checked and executed
# if it exists. If execution of the primitive is successful, the value is put
# on the stack, and execution of the method is finished.  If unsuccessful or
# absent (only in the case of a CompiledCode), the ruby code code
# in the CompiledCode is run.
module Rubinius
  class Executable
    attr_accessor :serial
    attr_accessor :primitive
    attr_accessor :tags

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

    ##
    # Allows instances of Executable and its subclasses
    # to duck-type as CompiledCode::Visibility since
    # they can both be entries in a MethodTable.
    def public?
      true
    end

    def private?
      false
    end

    alias_method :protected?, :private?
  end
end

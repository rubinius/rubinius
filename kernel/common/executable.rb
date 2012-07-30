# -*- encoding: us-ascii -*-

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

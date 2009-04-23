##
# A Selector knows all the SendSites that are for a particular method name.
# When a method is redefined the Selector is used to clear all the SendSites
# referencing the method's name.

module Rubinius
  class Selector

    ##
    # Clears this Selector's SendSites allowing each SendSite to discover a new
    # Executable.

    def clear
      Ruby.primitive :selector_clear
      raise PrimitiveFailure, "Selector#clear primitive failed"
    end
  end
end

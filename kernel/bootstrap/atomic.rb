module Rubinius
  class AtomicReference
    def initialize(val=nil)
      @value = val
    end

    attr_accessor :value

    alias_method :get, :value
    alias_method :set, :value=

    def compare_and_set(old, new)
      Rubinius.primitive :atomic_compare_and_set
      raise PrimitiveFailure, "compare_and_set failed"
    end

    def get_and_set(new)
      while true
        val = get
        return val if compare_and_set(val, new)
      end
    end
  end
end

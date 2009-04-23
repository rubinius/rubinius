module Rubinius
  class CompactLookupTable
    def self.allocate
      Ruby.primitive :compactlookuptable_allocate
      raise PrimitiveFailure, "CompactLookupTable.allocate primitive failed"
    end

    def [](key)
      Ruby.primitive :compactlookuptable_fetch
      raise PrimitiveFailure, "CompactLookupTable#[] primitive failed"
    end

    def []=(key, val)
      Ruby.primitive :compactlookuptable_store
      raise PrimitiveFailure, "CompactLookupTable#[]= primitive failed"
    end

    def key?(key)
      Ruby.primitive :compactlookuptable_has_key
      raise PrimitiveFailure, "CompactLookupTable#key? primitive failed"
    end

    def keys
      Ruby.primitive :compactlookuptable_keys
      raise PrimitiveFailure, "CompactLookupTable#keys primitive failed"
    end

    def values
      Ruby.primitive :compactlookuptable_values
      raise PrimitiveFailure, "CompactLookupTable#keys primitive failed"
    end
  end
end

module Rubinius
  class CompactLookupTable
    def self.allocate
      Rubinius.primitive :compactlookuptable_allocate
      raise PrimitiveFailure, "CompactLookupTable.allocate primitive failed"
    end

    def [](key)
      Rubinius.primitive :compactlookuptable_fetch
      raise PrimitiveFailure, "CompactLookupTable#[] primitive failed"
    end

    def []=(key, val)
      Rubinius.primitive :compactlookuptable_store
      raise PrimitiveFailure, "CompactLookupTable#[]= primitive failed"
    end

    def key?(key)
      Rubinius.primitive :compactlookuptable_has_key
      raise PrimitiveFailure, "CompactLookupTable#key? primitive failed"
    end

    def keys
      Rubinius.primitive :compactlookuptable_keys
      raise PrimitiveFailure, "CompactLookupTable#keys primitive failed"
    end

    def values
      Rubinius.primitive :compactlookuptable_values
      raise PrimitiveFailure, "CompactLookupTable#keys primitive failed"
    end
  end
end

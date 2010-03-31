module Rubinius

  # LoadedFeatures is a custom class for $LOADED_FEATURES. It presents an
  # Array interface but maintains an Array::IdentityMap for close to O(1)
  # lookup. Only the mutating methods actually used on $LOADED_FEATURES have
  # been implemented. Additional methods may need to be implemented in the
  # future.
  class LoadedFeatures < Array
    MIN_CAPACITY = 100

    def initialize(size_or_array=undefined, obj=undefined)
      super
      identity_map
    end

    def identity_map
      @identity_map = IdentityMap.new self, size + MIN_CAPACITY
    end
    private :identity_map

    # The IdentityMap must have sufficiently low load factor to operate well
    # so we remap if our size gets too large. This is a heuristic.
    def resize_map
      identity_map if size * 8 > @identity_map.capacity
    end
    private :resize_map

    def <<(obj)
      resize_map
      @identity_map.insert obj
      super
    end

    def clear
      super
      identity_map
    end

    def delete(obj)
      @identity_map.delete obj
      super
    end

    def delete_if
      super
      identity_map
      self
    end

    def include?(obj)
      @identity_map.include? obj
    end

    def unshift(*values)
      super
      identity_map
      self
    end
  end
end

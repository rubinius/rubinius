module Rubinius

  # LoadedFeatures is a custom class for $LOADED_FEATURES. It presents an
  # Array interface but maintains an Array::IdentityMap for close to O(1)
  # lookup. Only the mutating methods actually used on $LOADED_FEATURES have
  # been implemented. Additional methods may need to be implemented in the
  # future.
  class LoadedFeatures < Array

    def initialize(size_or_array=undefined, obj=undefined)
      super
      identity_map
    end

    def identity_map
      @identity_map = IdentityMap.from self
    end
    private :identity_map

    def <<(obj)
      # Someone changed something behind our back
      identity_map if size > @identity_map.size

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

    def replace(other)
      super
      identity_map
      self
    end
  end
end

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
      Rubinius.synchronize(self) do
        # Someone changed something behind our back
        identity_map if size > @identity_map.size

        @identity_map.insert obj
        super
      end
    end

    def clear
      Rubinius.synchronize(self) do
        super
        identity_map
      end
    end

    def delete(obj)
      Rubinius.synchronize(self) do
        @identity_map.delete obj
        super
      end
    end

    def delete_if
      Rubinius.synchronize(self) do
        super
        identity_map
      end

      self
    end

    def include?(obj)
      Rubinius.synchronize(self) do
        @identity_map.include? obj
      end
    end

    def unshift(*values)
      Rubinius.synchronize(self) do
        super
        identity_map
      end

      self
    end

    def pop(*v)
      Rubinius.synchronize(self) do
        ret = super
        identity_map
        ret
      end
    end

    def replace(other)
      Rubinius.synchronize(self) do
        super
        identity_map
      end
      self
    end
  end
end

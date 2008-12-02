# Provides a convenience method for accessing the metaclass
# of any object without overriding the method if it already
# exists in an implementation.
unless respond_to? :metaclass
  class Object
    def metaclass
      class << self
        self
      end
    end
  end
end

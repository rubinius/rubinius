# The Type module provides facilities for accessing various "type" related
# data about an object, as well as providing type coercion methods. These
# facilities are independent of the object and thus are more robust in the
# face of ad hoc monkeypatching.
module Rubinius
  module Type
    # Performs a direct kind_of? check on the object bypassing any method
    # overrides.
    def self.object_kind_of?(obj, cls)
      Rubinius.primitive :vm_object_kind_of
      raise PrimitiveFailure, "Rubinius::Type.kind_of failed"
    end

    def self.object_class(obj)
      Rubinius.primitive :vm_object_class
      raise PrimitiveFailure, "Rubinius::Type.object_class failed"
    end

    def self.object_singleton_class(obj)
      Rubinius.primitive :vm_object_singleton_class
      raise TypeError, "no singleton class available for a #{Type.object_class(obj)}"
    end

    def self.singleton_class_object(mod)
      Rubinius.primitive :vm_singleton_class_object
      raise PrimitiveFailure, "Rubinius::Type.singleton_class_object failed"
    end

    def self.object_respond_to?(obj, name)
      Rubinius.primitive :vm_object_respond_to
      raise PrimitiveFailure, "Rubinius::Type.object_respond_to? failed"
    end

    def self.object_equal(a, b)
      Rubinius.primitive :vm_object_equal
      raise PrimitiveFailure, "Rubinius::Type.object_equal failed"
    end
  end
end

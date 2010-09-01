class Class
  attr_accessor :instance_type

  attr_reader :instance_flags

  def self.allocate
    Ruby.primitive :class_s_allocate
    raise PrimitiveFailure, "Unable to create a new Class"
  end

  def set_superclass(sup)
    Ruby.primitive :class_set_superclass
    raise TypeError, "superclass must be a Class (#{Rubinius.object_class(sup)} given)"
  end

  def packed!(ary)
    Ruby.primitive :class_set_packed
    raise PrimitiveFailure, "Class#pack! failed"
  end

  def __metaclass_object__
    Ruby.primitive :class_get_metaclass_attached
    raise PrimitiveFailure, "Class#__metaclass_object__ failed"
  end
end

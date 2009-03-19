class Class
  attr_accessor :instance_type

  attr_reader :instance_flags

  def self.allocate
    Ruby.primitive :class_s_allocate
    raise PrimitiveFailure, "Unable to create a new Class"
  end

  def set_superclass(sup)
    Ruby.primitive :class_set_superclass
    raise PrimitiveFailure, "Unable to set the superclass"
  end
end

# TODO: is this obsolete?
class RuntimePrimitive < Executable
  attr_writer :variable

  def initialize(prim_name)
    @primitive = prim_name
    @serial = 0
  end
end

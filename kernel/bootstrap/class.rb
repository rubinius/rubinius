# -*- encoding: us-ascii -*-

class Class
  def self.allocate
    Rubinius.primitive :class_s_allocate
    raise PrimitiveFailure, "Class.allocate primitive failed"
  end

  def set_superclass(sup)
    Rubinius.primitive :class_set_superclass
    kind = Rubinius::Type.object_class(sup)
    raise TypeError, "Class.set_superclass: argument must be a Class (#{kind} given)"
  end

  private :set_superclass
end

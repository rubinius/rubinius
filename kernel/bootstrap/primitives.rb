# All primitives not else where in the bootstrap must be here.

class Object
  def become!(obj)
    Ruby.primitive :object_become
    raise PrimitiveFailure, "primitive failed"
  end
end

class IO
  def self.create_pipe(lhs, rhs)
    Ruby.primitive :create_pipe
    raise PrimitiveFailure, "primitive failed"
  end
  
  def reopen(other)
    Ruby.primitive :io_reopen
    raise ArgumentError, "only accepts an IO object"
  end
  
  def io_close
    Ruby.primitive :io_close
    raise PrimitiveFailure, "primitive failed"
  end
end

class CompiledMethod
  def self.load_from_file(path, version)
    Ruby.primitive :load_file
    raise PrimitiveFailure, "primitive failed"
  end

  def activate_as_script
    Ruby.primitive :activate_as_script
    raise PrimitiveFailure, "primitive failed"
  end
end

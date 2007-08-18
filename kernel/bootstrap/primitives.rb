# All primitives not else where in the bootstrap must be here.

class Object
  def fields
    Ruby.primitive :fields
  end

  def become!(obj)
    Ruby.primitive :object_become
  end
end

class IO
  def self.create_pipe(lhs, rhs)
    Ruby.primitive :create_pipe
  end
  
  def reopen(other)
    Ruby.primitive :io_reopen
    raise ArgumentError, "only accepts an IO object"
  end
  
  def io_close
    Ruby.primitive :io_close
  end
end

class Hash
  def delete_by_hash(hsh, key)
    Ruby.primitive :hash_delete
  end
end

class CompiledMethod
  def self.load_from_file(path, version)
    Ruby.primitive :load_file
  end
  
  def activate_as_script
    Ruby.primitive :activate_as_script
  end
end
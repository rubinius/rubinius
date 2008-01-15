module Archive
    
  def self.list_files(path)
    Ruby.primitive :archive_files
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.get_file(path, name)
    Ruby.primitive :archive_get_file
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.get_object(path, name, version)
    Ruby.primitive :archive_get_object
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.add_file(path, name, file)
    Ruby.primitive :archive_add_file
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.add_object(path, name, object, version)
    Ruby.primitive :archive_add_object
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.delete_file(path, name)
    Ruby.primitive :archive_delete_file
    raise PrimitiveFailure, "primitive failed"
  end
end

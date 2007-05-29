module Archive
    
  def self.list_files(path)
    Ruby.primitive :archive_files
  end
  
  def self.get_file(path, name)
    Ruby.primitive :archive_get_file
  end
  
  def self.get_object(path, name, version)
    Ruby.primitive :archive_get_object
  end
  
  def self.add_file(path, name, file)
    Ruby.primitive :archive_add_file
  end
  
  def self.add_object(path, name, object, version)
    Ruby.primitive :archive_add_object
  end
  
  def self.delete_file(path, name)
    Ruby.primitive :archive_delete_file
  end
end
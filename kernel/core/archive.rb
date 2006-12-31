module Archive
  def self.list_files(path)
    Ruby.primitive :archive_files
  end
  
  def self.get_file(path, name)
    Ruby.primitive :archive_get_file
  end
  
  def self.get_object(path, name)
    Ruby.primitive :archive_get_object
  end
end
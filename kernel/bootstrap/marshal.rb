module Marshal
  def self.dump(obj, version)
    Ruby.primitive :marshal_object
  end
  
  def self.dump_to_file(obj, path, version)
    Ruby.primitive :marshal_to_file
  end
  
  def self.load(str, version)
    Ruby.primitive :unmarshal_object
  end
  
  def self.load_from_file(path, version)
    Ruby.primitive :unmarshal_from_file
  end
end

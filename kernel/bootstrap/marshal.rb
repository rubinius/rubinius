module Marshal
  def self.dump_to_file(obj, path, version)
    Ruby.primitive :marshal_to_file
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.load_from_file(path, version)
    Ruby.primitive :unmarshal_from_file
    raise PrimitiveFailure, "primitive failed"
  end
end

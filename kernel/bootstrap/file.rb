class File < IO
  def self.unlink(path)
    Ruby.primitive :file_unlink
    raise PrimitiveFailure, "primitive failed"
  end
end

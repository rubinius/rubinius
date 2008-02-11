class File < IO  
  def self.unlink(path)
    Ruby.primitive :file_unlink
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.to_sexp_full(path, newlines)
    Ruby.primitive :file_to_sexp
    raise PrimitiveFailure, "primitive failed"
  end
end

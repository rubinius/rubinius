class File < IO  
  def self.unlink(path)
    Ruby.primitive :file_unlink
    raise PrimitiveFailure, "primitive failed"
  end

  def self.to_sexp_full(path, newlines)
    Ruby.primitive :file_to_sexp
    raise PrimitiveFailure, "primitive failed"
  end

  class Stat
    def self.stat(path, follow)
      Ruby.primitive :stat_file
      raise PrimitiveFailure, "File::Stat.basic_stat: failed"
    end
  end
end

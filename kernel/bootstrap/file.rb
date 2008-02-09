class File < IO  
  def self.unlink(path)
    Ruby.primitive :file_unlink
    raise PrimitiveFailure, "primitive failed"
  end

  def self.to_sexp_full(path, newlines)
    Ruby.primitive :file_to_sexp
    raise PrimitiveFailure, "primitive failed"
  end

  class BasicStat
    def self.basic_stat(path, follow)
      Ruby.primitive :basic_stat
      raise PrimitiveFailure, "File::Stat.basic_stat: failed"
    end
  end
end

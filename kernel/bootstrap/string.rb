class String
  def __symbol_lookup__
    Ruby.primitive :symbol_lookup
  end
  
  def to_sym
    __symbol_lookup__
  end
  
  def to_sexp_full(name, line, newlines)
    Ruby.primitive :string_to_sexp
  end
  
  def to_f
    Ruby.primitive :string_to_f
  end
  
  def __crypt__(other_str)
    Ruby.primitive :str_crypt
  end
  
  def append(str)
    Ruby.primitive :string_append
    
    raise TypeError, "only a String instance is accepted"
  end
  
  def dup
    Ruby.primitive :string_dup
  end

  def to_s
    self
  end
  
end

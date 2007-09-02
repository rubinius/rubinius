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
    unless str.kind_of? String
      raise TypeError, "only a String instance is accepted"
    end
    
    Ruby.asm "#local str\npush self\nstring_append\n"
  end
  
  def dup
    Ruby.asm "push self\nstring_dup\n"
  end

  def to_s
    self
  end
  
end

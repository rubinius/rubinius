class String
  def __symbol_lookup__
    Ruby.primitive :symbol_lookup
    raise PrimitiveFailure, "Unable to symbolize: #{self.dump}"
  end
  
  def to_sym
    __symbol_lookup__
  end
  
  def to_sexp_full(name, line, newlines)
    Ruby.primitive :string_to_sexp
    raise PrimitiveFailure, "primitive failed"
  end
  
  def to_f
    Ruby.primitive :string_to_f
    raise PrimitiveFailure, "primitive failed"
  end
  
  def __crypt__(other_str)
    Ruby.primitive :str_crypt
    raise PrimitiveFailure, "primitive failed"
  end
  
  def append(str)
    Ruby.primitive :string_append
    
    raise TypeError, "only a String instance is accepted"
  end
  
  def dup
    Ruby.primitive :string_dup
    raise PrimitiveFailure, "primitive failed"
  end

  def to_s
    self
  end
  
  def substring(start, count)
    return if count < 0 || start > @bytes || -start > @bytes

    start += @bytes if start < 0

    count = @bytes - start if start + count > @bytes
    count = 0 if count < 0

    str = self.class.allocate
    str.taint if self.tainted?
    if count == 0
      ba = ByteArray.new(0)
    else
      ba = @data.fetch_bytes(start, count)
    end

    str.initialize_from count, ba

    return str
  end
  
  def initialize_from(count, ba)
    @bytes = count
    @characters = count
    @data = ba
  end
  
  def ==(other)
    Ruby.primitive :string_equal
  end
  
  def length
    @bytes
  end

end

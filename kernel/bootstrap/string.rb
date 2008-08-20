class String
  def self.template(size, str)
    Ruby.primitive :string_template
    raise PrimitiveFailure, "String.template primitive failed"
  end

  def __symbol_lookup__
    Ruby.primitive :symbol_lookup
    raise PrimitiveFailure, "Unable to symbolize: #{self.dump}"
  end

  def to_sym
    __symbol_lookup__
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

  def copy_from(other, start, size, dest)
    Ruby.primitive :string_copy_from
    raise PrimitiveFailure, "String#copy_from primitive failed"
  end

  def substring(start, count)
    return if count < 0 || start > @bytes || -start > @bytes

    start += @bytes if start < 0
    count = @bytes - start if start + count > @bytes
    count = 0 if count < 0

    str = self.class.template count, 0
    str.copy_from self, start, count, 0
    str.taint if self.tainted?

    return str
  end

  def ==(other)
    Ruby.primitive :string_equal
  end

  def length
    @bytes
  end
end

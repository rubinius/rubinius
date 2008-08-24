class String
  def self.pattern(size, str)
    Ruby.primitive :string_pattern
    raise PrimitiveFailure, "String.pattern primitive failed"
  end

  # Returns the <code>Symbol</code> corresponding to <i>self</i>, creating the
  # symbol if it did not previously exist. See <code>Symbol#id2name</code>.
  #
  #   "Koala".intern         #=> :Koala
  #   s = 'cat'.to_sym       #=> :cat
  #   s == :cat              #=> true
  #   s = '@cat'.to_sym      #=> :@cat
  #   s == :@cat             #=> true
  #
  # This can also be used to create symbols that cannot be represented using the
  # <code>:xxx</code> notation.
  #
  #   'cat and dog'.to_sym   #=> :"cat and dog"
  #--
  # TODO: Add taintedness-check
  #++
  def to_sym
    Ruby.primitive :symbol_lookup
    raise PrimitiveFailure, "Unable to symbolize: #{self.dump}"
  end

  def to_f
    Ruby.primitive :string_to_f
    raise PrimitiveFailure, "primitive failed"
  end

  def __crypt__(other_str)
    Ruby.primitive :string_crypt
    raise PrimitiveFailure, "String#crypt primitive failed"
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
    return if count < 0 || start > @num_bytes || -start > @num_bytes

    start += @num_bytes if start < 0
    count = @num_bytes - start if start + count > @num_bytes
    count = 0 if count < 0

    str = self.class.pattern count, 0
    str.copy_from self, start, count, 0
    str.taint if self.tainted?

    return str
  end

  def ==(other)
    Ruby.primitive :string_equal
  end

  def length
    @num_bytes
  end
end

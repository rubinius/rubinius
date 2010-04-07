class String
  def self.pattern(size, str)
    Ruby.primitive :string_pattern
    raise PrimitiveFailure, "String.pattern primitive failed"
  end

  def to_f
    Ruby.primitive :string_to_f
    raise PrimitiveFailure, "String#to_f primitive failed"
  end

  alias_method :convert_float, :to_f

  def __crypt__(other_str)
    Ruby.primitive :string_crypt
    raise PrimitiveFailure, "String#crypt primitive failed"
  end

  def append(str)
    Ruby.primitive :string_append
    raise TypeError, "String#append primitive only accepts Strings"
  end

  def dup
    Ruby.primitive :string_dup
    raise PrimitiveFailure, "String#dup primitive failed"
  end

  def copy_from(other, start, size, dest)
    Ruby.primitive :string_copy_from
    raise PrimitiveFailure, "String#copy_from primitive failed"
  end

  def substring(start, count)
    Ruby.primitive :string_substring

    return nil if count < 0

    if start < 0
      start += @num_bytes
      return nil if start < 0
    else
      return nil if start > @num_bytes
    end

    count = @num_bytes - start if start + count > @num_bytes
    count = 0 if count < 0

    str = self.class.pattern count, 0
    str.copy_from self, start, count, 0
    str.taint if self.tainted?

    return str
  end

  def find_string(pattern, start)
    Ruby.primitive :string_index
    raise PrimitiveFailure, "String#find_string failed"
  end

  private :find_string

  def find_string_reverse(pattern, start)
    Ruby.primitive :string_rindex
    raise PrimitiveFailure, "String#find_string_reverse failed"
  end

  def ==(other)
    Ruby.primitive :string_equal
    raise PrimitiveFailure, "String#== failed"
  end

  # Returns the length of <i>self</i>.
  def length
    @num_bytes
  end

  alias_method :size, :length

  def find_character(offset)
    Ruby.primitive :string_find_character
    raise PrimitiveFailure, "String#find_character failed"
  end

  # In time, the JIT should be able to handle this as a ruby method.
  def transform(tbl, respect_kcode)
    Ruby.primitive :string_transform
    raise PrimitiveFailure, "String#transform failed"
  end

  def self.base64_encode(str, line_length)
    Ruby.primitive :string_base64_encode
    raise PrimitiveFailure, "String.base64_encode failed"
  end

  def self.base64_decode(str)
    Ruby.primitive :string_base64_decode
    raise PrimitiveFailure, "String.base64_decode failed"
  end
end

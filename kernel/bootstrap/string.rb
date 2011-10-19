class String
  def self.pattern(size, str)
    Rubinius.primitive :string_pattern
    raise PrimitiveFailure, "String.pattern primitive failed"
  end

  def to_f
    Rubinius.primitive :string_to_f
    raise PrimitiveFailure, "String#to_f primitive failed"
  end

  alias_method :convert_float, :to_f

  def __crypt__(other_str)
    Rubinius.primitive :string_crypt
    raise PrimitiveFailure, "String#crypt primitive failed"
  end

  def append(str)
    Rubinius.primitive :string_append
    raise TypeError, "String#append primitive only accepts Strings"
  end

  def dup
    Rubinius.primitive :string_dup
    raise PrimitiveFailure, "String#dup primitive failed"
  end

  def copy_from(other, start, size, dest)
    Rubinius.primitive :string_copy_from
    raise PrimitiveFailure, "String#copy_from primitive failed"
  end

  def substring(start, count)
    Rubinius.primitive :string_substring

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
    Rubinius.primitive :string_index
    raise PrimitiveFailure, "String#find_string failed"
  end

  def find_string_reverse(pattern, start)
    Rubinius.primitive :string_rindex
    raise PrimitiveFailure, "String#find_string_reverse failed"
  end

  def ==(other)
    Rubinius.primitive :string_equal
    raise PrimitiveFailure, "String#== failed"
  end

  def secure_compare(other)
    Rubinius.primitive :string_secure_compare

    if other.kind_of?(String)
      raise PrimitiveFailure, "String#secure_compare failed"
    else
      secure_compare StringValue(other)
    end
  end

  # Returns the length of <i>self</i>.
  attr_reader_specific :num_bytes, :length
  alias_method :size, :length

  def find_character(offset)
    Rubinius.primitive :string_find_character
    raise PrimitiveFailure, "String#find_character failed"
  end

  # In time, the JIT should be able to handle this as a ruby method.
  def transform(tbl, respect_kcode)
    Rubinius.primitive :string_transform
    raise PrimitiveFailure, "String#transform failed"
  end

  def resize_capacity(count)
    Rubinius.primitive :string_resize_capacity
    raise PrimitiveFailure, "String#resize_capacity failed"
  end

end

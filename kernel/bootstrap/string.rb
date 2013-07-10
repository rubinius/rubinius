# -*- encoding: us-ascii -*-

class String
  attr_reader :num_bytes
  attr_reader_specific :num_bytes, :bytesize

  def self.pattern(size, str)
    Rubinius.primitive :string_pattern
    raise PrimitiveFailure, "String.pattern primitive failed"
  end

  def to_f
    Rubinius::Type::coerce_to_float self, false, false
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

  def byte_append(str)
    Rubinius.primitive :string_byte_append
    raise TypeError, "String#byte_append primitive only accepts Strings"
  end

  def byteslice(index_or_range, length=undefined)
    Rubinius.primitive :string_byte_substring

    if index_or_range.kind_of? Range
      index = Rubinius::Type.coerce_to index_or_range.begin, Fixnum, :to_int
      index += @num_bytes if index < 0
      return if index < 0 or index > @num_bytes

      finish = Rubinius::Type.coerce_to index_or_range.end, Fixnum, :to_int
      finish += @num_bytes if finish < 0

      finish += 1 unless index_or_range.exclude_end?
      length = finish - index

      return byteslice 0, 0 if length < 0
    else
      index = Rubinius::Type.coerce_to index_or_range, Fixnum, :to_int
      index += @num_bytes if index < 0

      if undefined.equal?(length)
        return if index == @num_bytes
        length = 1
      else
        length = Rubinius::Type.coerce_to length, Fixnum, :to_int
        return if length < 0
      end

      return if index < 0 or index > @num_bytes
    end

    byteslice index, length
  end

  def dup
    other = Rubinius.invoke_primitive :string_dup, self
    Rubinius.privately do
      other.initialize_copy self
    end
    other
  end

  def substring(start, count)
    Rubinius.primitive :string_substring
    raise PrimitiveFailure, "String#substring primitive failed"
  end

  def find_string(pattern, start)
    Rubinius.primitive :string_index
    raise PrimitiveFailure, "String#find_string primitive failed"
  end

  def find_string_reverse(pattern, start)
    Rubinius.primitive :string_rindex
    raise PrimitiveFailure, "String#find_string_reverse primitive failed"
  end

  def ==(other)
    Rubinius.primitive :string_equal
    raise PrimitiveFailure, "String#== primitive failed"
  end

  def secure_compare(other)
    Rubinius.primitive :string_secure_compare

    if other.kind_of?(String)
      raise PrimitiveFailure, "String#secure_compare primitive failed"
    else
      secure_compare StringValue(other)
    end
  end

  def find_character(offset)
    Rubinius.primitive :string_find_character
    raise PrimitiveFailure, "String#find_character primitive failed"
  end

  def size
    Rubinius.primitive :string_size
    raise PrimitiveFailure, "String#size primitive failed"
  end

  alias_method :length, :size

  # This is a work-in-progress. String is entirely coded around the idea of
  # bytes, but we have to convert to the idea of characters, even for 1.8
  # mode, where UTF-8, EUC, and SJIS will be represented properly as encoded
  # strings rather than some ad hoc internal state. However, the byte
  # characteristic of String is still important. More work remains.
  def num_bytes=(bytes)
    @num_chars = nil
    @num_bytes = bytes
  end

  # In time, the JIT should be able to handle this as a ruby method.
  def transform(tbl, respect_kcode)
    Rubinius.primitive :string_transform
    raise PrimitiveFailure, "String#transform primitive failed"
  end
end

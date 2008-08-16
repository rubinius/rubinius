class ByteArray
  def self.allocate(cnt)
    Ruby.primitive :bytearray_allocate
    raise PrimitiveFailure, "primitive failed"
  end

  def self.new(cnt)
    obj = allocate(cnt)
    Rubinius.asm(obj) do |obj|
      push_block
      run obj
      send_with_block :initialize, 0, true
    end

    return obj
  end

  def fetch_bytes(start, count)
    Ruby.primitive :bytearray_fetch_bytes
    raise PrimitiveFailure, "primitive failed"
  end

  def move_bytes(start, count, dest)
    Ruby.primitive :bytearray_move_bytes
    raise ArgumentError, "move_bytes failed!"
  end

  def get_byte(index)
    Ruby.primitive :bytearray_get_byte
    raise PrimitiveFailure, "primitive failed"
  end

  def set_byte(index, value)
    Ruby.primitive :bytearray_set_byte
    raise PrimitiveFailure, "primitive failed"
  end

  def compare_bytes(other, a, b)
    Ruby.primitive :bytearray_compare_bytes
    raise PrimitiveFailure, "primitive failed"
  end

  def size
    Ruby.primitive :bytearray_size
    raise PrimitiveFailure, "primitive failed"
  end

  def dup_into(other)
    Ruby.primitive :bytes_dup_into
    raise PrimitiveFailure, "primitive failed"
  end

  def dup(cls=nil)
    cls ||= self.class
    obj = cls.new(self.size)
    dup_into obj
    return obj
  end
end

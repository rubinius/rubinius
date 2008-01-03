class ByteArray
  def self.allocate(cnt)
    Ruby.primitive :allocate_bytes
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
    Ruby.primitive :fetch_bytes
  end

  def move_bytes(start, count, offset)
    Ruby.primitive :move_bytes
    raise ArgumentError, "move_bytes failed!"
  end

  def get_byte(index)
    Ruby.primitive :get_byte
  end
  
  def set_byte(index, value)
    Ruby.primitive :set_byte
  end

  def <=>(other)
    Ruby.primitive :compare_bytes
  end  

  def size
    Ruby.primitive :bytearray_size
  end
  
  def dup_into(other)
    Ruby.primitive :bytes_dup_into
  end
  
  def dup(cls=nil)
    cls ||= self.class
    obj = cls.new(self.size)
    dup_into obj
    return obj
  end
end

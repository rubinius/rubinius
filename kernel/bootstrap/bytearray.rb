class ByteArray
  def self.new(cnt)
    Ruby.primitive :allocate_bytes
  end

  def fetch_bytes(start, count)
    Ruby.primitive :fetch_bytes
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

  def each
    0.upto(self.size - 1) do |i|
      yield get_byte(i)
    end
  end
  
  def dup_into(other)
    Ruby.primitive 32 # :bytes_dup_into
  end
  
  def dup(cls=nil)
    cls ||= self.class
    obj = cls.new(self.size)
    dup_into obj
    return obj
  end
  
  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)} #{size} bytes>"
  end
end

class String
  def to_s
    self
  end
  
  def inspect
    "\"#{to_s}\""
  end
  
  def size
    bytes
  end
  
  def <<(other)
    out = nil
    Ruby.asm "push other\npush self\nstring_append\nset out"
    return out
  end
  
  def dup
    out = nil
    Ruby.asm "push self\nstring_dup\nset out"
    return out
  end
  
  def substring(start, count)
    nd = self.data.fetch_bytes(start, count)
    sz = count - start
    str = String.allocate
    str.put 0, sz
    str.put 1, sz
    str.put 3, nd
    return str
  end
  
  def ==(other)
    (self.data <=> other.data) == 0
  end
  
  def prefix?(pre)
    return false if pre.size >= self.size
    sub = substring(0, pre.size)
    pre == sub
  end
  
  def to_sym
    Ruby.primitive :symbol_lookup
  end
end

class ByteArray
  def self.new(cnt)
    Ruby.primitive :allocate_bytes
  end
  
  def fetch_bytes(start, count)
    Ruby.primitive :fetch_bytes
  end
  
  def <=>(other)
    Ruby.primitive :compare_bytes
  end  
end
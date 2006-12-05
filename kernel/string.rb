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
    str = String.allocate
    str.put 0, count
    str.put 1, count
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

  def to_sexp_full(name, line, newlines)
    Ruby.primitive :string_to_sexp
  end

  def to_sexp(name="(eval)",line=1,newlines=true)
    out = to_sexp_full(name, line, newlines)
    if out.kind_of? Tuple
      exc = SyntaxError.new out.at(0)
      exc.put 2, out.at(1)
      exc.put 3, out.at(2)
      raise exc
    end
    return out
  end
  
  def *(num)
    str = []
    num.times { str << self }
    return str.join("")
  end
end

class SyntaxError
  self.instance_fields = 4
  def column
    at(2)
  end

  def line
    at(3)
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

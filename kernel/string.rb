class String
  def to_s
    self
  end

  def inspect
    "\"#{to_s}\""
  end

  def size
    @bytes
  end

  def length
    @bytes
  end

  def <<(other)
    out = nil
    Ruby.asm "push other\npush self\nstring_append\nset out"
    return out
  end

  def +(other)
    o = self.dup
    o << other
    return o
  end

  def dup
    out = nil
    Ruby.asm "push self\nstring_dup\nset out"
    return out
  end

  def substring(start, count)
    nd = @data.fetch_bytes(start, count)
    str = String.allocate
    str.put 0, count
    str.put 1, count
    str.put 3, nd
    return str
  end

  # FIXME - Make Unicode-safe
  def codepoints
    chars = []
    @bytes.times do |pos|
      chars << self.substring(pos, 1)
    end
    chars
  end

  def ==(other)
    (@data <=> other.data) == 0
  end

  def prefix?(pre)
    return false if pre.size >= @size
    sub = substring(0, pre.size)
    pre == sub
  end

  def to_sym
    Ruby.primitive :symbol_lookup
  end
  alias :intern :to_sym

  def to_sexp_full(name, line, newlines)
    Ruby.primitive :string_to_sexp
  end

  def to_sexp(name="(eval)",line=1,newlines=true)
    out = to_sexp_full(name, line, newlines)
    if out.kind_of? Tuple
      exc = SyntaxError.new out.at(0)
      exc.import_position out.at(1), out.at(2)
      raise exc
    end
    return out
  end

  def *(num)
    str = []
    num.times { str << self }
    return str.join("")
  end

  def strip
    r = /\s*([^\s].*[^\s])\s*/m
    m = r.match(self)
    return m.captures[0]
  end

  def =~(pattern)
    m = pattern.match(self)
    m ? m.full.at(0) : nil 
  end

  def [](arg, len = nil)
    if len
      len = len.to_i
      return nil if len < 0
    end
    arg = Regexp.new(Regexp.escape(arg)) if arg.is_a?(String)
    size = self.size
    
    if arg.respond_to? :match
      m = arg.match(self)
      m[len.to_i] if m
    elsif arg.respond_to?(:first) && arg.respond_to?(:last)
      from = arg.first
      to = arg.last
      to = to - 1 if arg.respond_to?(:exclude_end?) && arg.exclude_end?
      from = from + size if from < 0
      to = to + size if to < 0
      len = to - from + 1
      self[from, len]
      
    elsif arg && arg.respond_to?(:to_i)
      arg = arg.to_i
      arg = arg + size if arg < 0
      if 0 <= arg && arg < size
        if len
          len = size - arg if arg + len >= size
          substring(arg, len)
        else
          @data.get_byte(arg)
        end
      else # invalid start index
        len ? "" : nil
      end
    else
      raise ArgumentError.new("String#[] cannot accept #{arg.class} objects")
    end
  end

  alias_method :slice, :[]

  def each_byte(&prc)
    @data.each(&prc)
  end

end

class SyntaxError
  self.instance_fields = 4

  ivar_as_index :column => 2, :line => 3

  def column
    @column
  end

  def line
    @line
  end

  def import_position(c,l)
    @column = c
    @line = l
  end
end

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
  
  def <=>(other)
    Ruby.primitive :compare_bytes
  end  

  def size
    Ruby.primitive :bytearray_size
  end

  def each
    size = self.size
    i = 0
    while i <= size
      yield get_byte(i)
      i += 1
    end
  end
  
end

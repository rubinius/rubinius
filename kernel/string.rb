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
    arg = Regexp.escape(arg) if arg.is_a?(String)
    case arg
    when Regexp
      m = arg.match(self)
      return nil unless m
      match_start, match_end = m.full.at(0), m.full.at(1)
      count = match_end - match_start
      substring match_start, count
    when Numeric
      if len
        substring arg, len
      else
        @data.get_byte(arg)
      end
    when Range
      raise NotImplementedError.new("String#[] does not yet accept Range arguments")
    else
      raise ArgumentError.new("String#[] cannot accept #{arg.class} objects")
    end
  end

  alias_method :slice, :[]
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
end

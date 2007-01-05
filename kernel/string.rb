class String
  def to_s
    self
  end
  
  ControlCharacters = [?\n, ?\t, ?\a, ?\b, ?\v, ?\f, ?\r, ?\e]
  ControlPrintValue = ["\\n", "\\t", "\\a", "\\b", "\\v", "\\f", "\\r", "\\e"]
  def inspect
    res =  "\""
    0.upto(@bytes - 1) do |idx|
      char = @data.get_byte(idx)
      if ci = ControlCharacters.index(char)
        res << ControlPrintValue[ci]
      elsif char == ?"
        res << "\\\""
      elsif char == ?\\
        res << "\\\\"
      elsif char < 32 or char > 126
        v = char.to_s(8)
        if v.size == 1
          res << "\\00#{v}"
        elsif v.size == 2
          res << "\\0#{v}"
        else
          res << "\\#{v}"
        end
      else
        res << char.chr
      end
    end
    res << "\""
    return res
  end

  def size
    @bytes
  end

  def length
    @bytes
  end

  def <<(other)
    unless other.kind_of? String
      other = other.to_str
    end
    
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
    return false unless String === other
    (@data <=> other.data) == 0
  end
  
  alias :=== :==

  def <=>(other)
    @data <=> other.data
  end

  def prefix?(pre)
    return false if pre.size > @bytes
    sub = substring(0, pre.size)
    pre == sub
  end
  
  def suffix?(pre)
    return false if pre.size >= @bytes
    sub = substring(@bytes - pre.size, pre.size)
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

  def reverse
    str = ""
    i = @bytes - 1
    while i >= 0
      str << self[i,1]
      i -= 1
    end
    return str
  end

  def reverse!
    sd = self.reverse
    @data = sd.data
    @bytes = sd.size
    return self
  end

  def strip
    r = /\s*([^\s].*[^\s])\s*/m
    m = r.match(self)
    return m.captures[0]
  end
  
  def strip!
    sd = self.strip
    @data = sd.data
    @bytes = sd.size
    return self
  end

  def =~(pattern)
    m = pattern.match(self)
    m ? m.full.at(0) : nil 
  end

  def include?(arg)
    if arg.is_a? Fixnum
      @data.each { |b|  return true if b == arg }
      return false
    elsif arg.is_a? String
      return (self.index(arg) != nil) 
    else
      raise ArgumentError.new("String#include? cannot accept #{arg.class} objects")
    end
  end

  def index(arg, offset = nil )
    if arg.is_a? Fixnum
      i = 0
      @data.each { |b| return i if b == arg; i += 1 }
    elsif arg.is_a? String
      argsize = arg.size
      max = self.size - argsize
      if max >= 0 and argsize > 0
        0.upto(max) do |i|
          if @data.get_byte(i) == arg.data.get_byte(0)
            return i if substring(i,argsize) == arg
          end
        end
      end
    else
      raise ArgumentError.new("String#index cannot accept #{arg.class} objects")
    end
    return nil 
  end

  def [](arg, len = nil)
    if len
      len = len.to_i
      return nil if len < 0
    end

    if arg.is_a? String
      unless len.nil?
        raise ArgumentError.new("String#[] got incorrect arguments.") # TODO: Make this helpful.
      end
      return (self.include?(arg) ? arg.dup : nil)
    elsif arg.respond_to? :match
      m = arg.match(self)
      m[len.to_i] if m
    elsif arg.respond_to?(:first) and arg.respond_to?(:last)
      from = arg.first
      to = arg.last
      to = to - 1 if arg.respond_to?(:exclude_end?) && arg.exclude_end?
      size = self.size
      from = from + size if from < 0
      to = to + size if to < 0
      len = to - from + 1
      self[from, len]
      
    elsif arg and arg.respond_to?(:to_i)
      arg = arg.to_i
      size = self.size
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
  
  def empty?
    @bytes == 0
  end
  
  def split(pattern, limit=nil)
    return [] if @bytes == 0
    return [self] if limit == 0
    
    ret = []
    count = 0
    pattern = Regexp.new Regexp.quote(pattern) unless Regexp === pattern
    str = self
    while match = pattern.match(str)
      count += 1
      ret << match.pre_match
      str = match.post_match
      
      return ret if limit and limit == count
    end
    
    unless str.empty?
      ret << str
    end
    
    return ret
  end
  
  def to_i(radix=10)
    if self[0] == ?-
      neg = true
      i = 1
    else
      neg = false
      i = 0
    end
    
    ret = 0
    i.upto(@bytes - 1) do |idx|
      char = @data.get_byte(idx)
      if char >= ?0 and char <= ?9
        ret *= radix
        ret += (char - ?0)
      # An invalid character.
      elsif char != ?_
        return ret
      end
      
    end
    
    ret = -ret if neg
    
    return ret
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
    0.upto(self.size - 1) do |i|
      yield get_byte(i)
    end
  end
end

class String
  def __ivars__; nil; end

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
      if other.is_a?(Integer)
        other = other.chr
      else
        other = other.to_str
      end
    end
    
    out = nil
    Ruby.asm "push other\npush self\nstring_append\nset out"
    return out
  end
  alias :concat :<<

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
    if String === other
      return (@data <=> other.data) == 0
    elsif other.respond_to?(:to_str)
      return other == self
    end
    false
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

  def each_byte(&prc)
    0.upto(@bytes - 1) do |i|
      yield @data.get_byte(i)
    end
  end
  
  def empty?
    @bytes == 0
  end
  
  def split(pattern=nil, limit=nil)
    return [] if @bytes == 0
    return [self] if limit == 0
    pattern = $; if pattern.nil?
    pattern = /\s+/ if pattern.nil?
    pattern = Regexp.new Regexp.quote(pattern) unless Regexp === pattern
    
    ret = []
    count = 0
    str = self
    while match = pattern.match(str)
      count += 1
      ret << match.pre_match
      str = match.post_match
      return ret if limit and limit == count
    end

    ret << str unless str.empty?
    return ret
  end
  

  # TODO: check that the string will never go over the maximum range
  #       as the function is not supposed to raise an exception.
  def to_i(radix=10)
    return 0 unless @bytes > 0
    
    # leading whitespace removal
    i = 0
    while i < @bytes
      break unless self[i].isspace
      i += 1
    end
    return 0 if i >= @bytes

    # Sign determination
    if self[i] == ?-
      neg = true
      i += 1
    else
      neg = false
      if self[i] == ?+
        i += 1
      end
    end
    
    # Determine the radix from the string for radix = 0
    # 0b = 2, 0o = 8, 0x = 16, defaults to radix = 10
    if @bytes - i >= 2
      z = self[i]
      f = self[i+1].tolower
      if radix == 0
        radix = 10
        if z == ?0
          if f == ?b
            radix = 2
          elsif f == ?o
            radix = 8
          elsif f == ?x
            radix = 16
          else
            radix = 8
          end
        end
      end
      if z == ?0
        if (radix == 2 and f == ?b) or
           (radix == 8 and f == ?o) or
           (radix == 16 and f == ?x)
          i += 2
        end
      end
    end

    ret = 0
    i.upto(@bytes - 1) do |idx|
      char = @data.get_byte(idx)
      value = 0
      if char >= ?0 and char <= ?9
        value = (char - ?0)
      elsif char >= ?A and char <= ?Z
        value = (char - ?A + 10)
      elsif char >= ?a and char <= ?z
        value = (char - ?a + 10)
      # An invalid character.
      elsif char != ?_
        return neg ? -ret : ret
      end

      if value >= radix
        return neg ? -ret : ret
      end

      ret *= radix
      ret += value
    end
    
    return neg ? -ret : ret
  end
end

class String
  ivar_as_index :bytes => 0, :characters => 1, :encoding => 2, :data => 3
  def bytes     ; @bytes      ; end
  def characters; @characters ; end
  def encoding  ; @encoding   ; end
  def data      ; @data       ; end
  def __ivars__ ; nil         ; end

  def to_s
    self
  end
  
  ControlCharacters = [?\n, ?\t, ?\a, ?\v, ?\f, ?\r, ?\e, ?\b]
  ControlPrintValue = ["\\n", "\\t", "\\a", "\\v", "\\f", "\\r", "\\e", "\\b"]

  # Returns a printable version of _str_, with special characters
  # escaped.
  #
  #   str = "hello"
  #   str[3] = 8
  #   str.inspect       #=> "hel\010o"
  def inspect
    res =  "\""
    self.each_byte do |char|
      if ci = ControlCharacters.index(char)
        res << ControlPrintValue[ci]
      elsif char == ?"
        res << "\\\""
      elsif char == ?\\
        res << "\\\\"
      elsif char == ?#
        res << "\\\#"
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

  def length
    @bytes
  end
  alias :size :length

  # Append --- Concatenates the given object to <i>self</i>. If the object is a
  # <code>Fixnum</code> between 0 and 255, it is converted to a character before
  # concatenation.
  #    
  #   a = "hello "
  #   a << "world"   #=> "hello world"
  #   a.concat(33)   #=> "hello world!"
  def <<(other)
    unless other.kind_of? String
      if other.is_a?(Integer) && other >= 0 && other <= 255
        other = other.chr
      elsif other.respond_to? :to_str
        other = other.to_str
      else
        raise TypeError, "can't convert #{other.class} into String"
      end
    end
    
    Ruby.asm "#local other\npush self\nstring_append\n"
  end
  alias :concat :<<

  # Concatenation --- Returns a new <code>String</code> containing
  # <i>other</i> concatenated to <i>string</i>.
  # 
  #   "Hello from " + self.to_s   #=> "Hello from main"
  def +(other)
    self.dup << other
  end

  def dup
    Ruby.asm "push self\nstring_dup\n"
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
      return false unless @bytes == other.size
      
      # This clamps the data to the right size, then we compare
      # FIXME: This is very inefficient, creating a new ByteArray just
      # to compare. 
      ld = @data.fetch_bytes(0, @bytes)
      rd = other.data.fetch_bytes(0, @bytes)
      
      return (ld <=> rd) == 0
      
      # It would be nice if the rest of this worked, but it had problems
      # last I (evan) tried. We need to go through and make sure null termination
      # of ByteArray's is universal.
      od = other.data
      if @data.size == od.size
        return (@data <=> od) == 0
      elsif @data.size < od.size
        puts "od is bigger"
        return (od.fetch_bytes(0, @data.size) <=> @data) == 0
      else
        puts "data is bigger"
        out = (@data.fetch_bytes(0, od.size) <=> od)
        p out
        return out == 0
      end
    elsif other.respond_to?(:to_str)
      return other == self
    end
    false
  end
  
  alias :=== :==

  def <=>(other)
    if String === other
      len = other.size
      len = size if size < len

      od = other.data
      len.times do |i|
        result = (@data.get_byte(i) <=> od.get_byte(i))
        return result if result != 0
      end

      if size < other.size
        return -1
      elsif size > other.size
        return 1
      else
        return 0
      end
    else
      return unless other.respond_to?(:to_str) && other.respond_to?(:<=>)
      
      tmp = other <=> self
      return unless tmp
      
      tmp = Integer(tmp) unless tmp.is_a?(Integer)
      return -(tmp)
    end
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

  def __symbol_lookup__
    Ruby.primitive :symbol_lookup
  end

  def to_sym
    raise ArgumentError, "interning empty string" if self.empty?
    __symbol_lookup__
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
  
  def split(pattern = $;, limit = nil)
    return [] if @bytes == 0
    return [self] if limit == 0
    pattern = /\s+/ if pattern.nil?
    pattern = Regexp.new(Regexp.quote(pattern)) unless Regexp === pattern
    
    ret = []
    count = 0
    str = self
    while match = pattern.match(str)
      # When the Regexp matches a zero-length string, return an Array of characters
      if match.begin(0) == match.end(0)
        cp = self.codepoints
        if limit
          return cp[0,limit-1] + self[limit-1,@bytes]
        else
          return cp
        end
      end

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
    unless radix.is_a? Integer
      raise TypeError, "can't convert #{radix.class} into Integer" unless radix.respond_to? :to_int
      radix = radix.to_int
    end
    
    raise ArgumentError, "illegal radix #{radix}" if radix < 0
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


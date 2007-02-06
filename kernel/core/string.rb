class String
  include Comparable, Enumerable
  
  alias_method :to_str, :to_s

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
  
  def replace(other)
    @data = other.dup.data
    @bytes = other.bytes
    @characters = other.characters
    @encoding = other.encoding
    self
  end
  
  def replace_if(other)
    self == other ? nil : replace(other)
  end
  
  def chomp
    return "" unless @bytes > 0
    i = @bytes-1
    i -= 1 if @data.get_byte(i) == ?\n
    i -= 1 if @data.get_byte(i) == ?\r and i > 0
    substring(0, i+1)
  end
  
  def chomp!
    replace_if(chomp)
  end
  
  def chop
    count = @bytes > 0 ? @bytes-1 : 0
    substring(0, count)
  end
  
  def chop!
    replace_if(chop)
  end
  
  def capitalize
    str = self.dup
    c = str.data.get_byte(0)
    str.data.set_byte(0, c.toupper)
    i = 1
    while i < str.bytes
      c = str.data.get_byte(i)
      str.data.set_byte(i, c.tolower)
      i += 1
    end
    str
  end
  
  def capitalize!
    replace_if(capitalize)
  end
  
  def swapcase
    str = self.dup
    i = 0
    d = str.data
    while i < str.bytes
      c = d.get_byte(i)
      d.set_byte(i, c.toupper) if c.islower
      d.set_byte(i, c.tolower) if c.isupper
      i += 1
    end
    str
  end
  
  def swapcase!
    replace_if(swapcase)
  end
  
  def upcase
    str = self.dup
    i = 0
    while i < str.bytes
      c = str.data.get_byte(i)
      str.data.set_byte(i, c.toupper)
      i += 1
    end
    str
  end
  
  def upcase!
    replace_if(upcase)
  end
  
  def downcase
    str = self.dup
    i = 0
    while i < str.bytes
      c = str.data.get_byte(i)
      str.data.set_byte(i, c.tolower)
      i += 1
    end
    str
  end
  
  def downcase!
    replace_if(downcase)
  end

  def reverse
    str = self.dup
    i = 0
    j = str.bytes - 1
    d = str.data
    while i < j
      a = d.get_byte(i)
      b = d.get_byte(j)
      d.set_byte(j, a)
      d.set_byte(i, b)
      i += 1
      j -= 1
    end
    str
  end

  def reverse!
    replace_if(reverse)
  end
  
  def lstrip
    i = 0
    while i < @bytes
      c = @data.get_byte(i)
      if c.isspace or c == 0
        i += 1
      else
        break
      end
    end
    str = self.dup
    str.substring(i, @bytes - i)
  end
  
  def lstrip!
    replace_if(lstrip)
  end
  
  def rstrip
    i = @bytes - 1
    while i >= 0
      c = @data.get_byte(i)
      if c.isspace or c == 0
        i -= 1
      else
        break
      end
    end
    str = self.dup
    str.substring(0, i+1)
  end
  
  def rstrip!
    replace_if(rstrip)
  end

  def strip
    str = lstrip
    str.rstrip
  end
  
  def strip!
    replace_if(strip)
  end

  def gsub(pattern, rep=nil)
    str = self.dup
    out = ""
    pattern = Regexp.new(pattern) if String === pattern

    if block_given?
      while m = pattern.match(str)
        out << str[0...m.begin(0)] if m.begin(0) > 0
        out << yield(m[0])
        str = str[m.end(0)..-1]
      end
    else
      raise ArgumentError, "wrong number of (1 for 2)" if rep == nil
      while m = pattern.match(str)
        out << str[0...m.begin(0)] if m.begin(0) > 0
        out << rep.gsub(/\\\d/) { |x| m[x[0] - ?0] }
        str = str[m.end(0)..-1]
      end
    end
    return out << str
  end

  def gsub!(pattern, rep=nil)
    replace_if(gsub(pattern, rep))
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
      return (nil != self.index(arg)) 
    else
      raise ArgumentError.new("String#include? cannot accept #{arg.class} objects")
    end
  end

  def index(arg, offset = nil )
    if arg.is_a? Fixnum
      i = 0
      @data.each { |b| return i if b == arg; i += 1 }
    elsif arg.is_a? String
      idx = 0
      if offset
        if offset >= 0
          return nil if offset >= self.size
          idx = offset
        else
          return nil if (1-offset) >= self.size
          idx = self.size + offset
        end
      end
      argsize = arg.size
      max = self.size - argsize
      if max >= 0 and argsize > 0
        idx.upto(max) do |i|
          if @data.get_byte(i) == arg.data.get_byte(0)
            return i if substring(i,argsize) == arg
          end
        end
      end
    elsif arg.is_a? Regexp
      idx = offset ? offset : 0
      mstr = self[idx..-1]
      offset = self.size - mstr.size
      m = arg.match(mstr)
      if m
        return offset + m.begin(0)
      end
      return nil
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
        raise ArgumentError.new("String#[] cannot accept a second argument with a String.")
      end
      return (self.include?(arg) ? arg.dup : nil)
    elsif arg.respond_to? :match
      m = arg.match(self)
      return m[len.to_i] if m && len
      return m[0] if m
      return nil
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

  def []=(idx, ent, *args)
    cnt = nil
    if args.size != 0
      cnt = ent
      ent = args[0]             # 2nd arg (cnt) is the optional one!
    end

    if idx.class == Range
      if cnt
        raise ArgumentError, "Second argument invalid with a range"
      end
      lst = idx.last
      if lst < 0
        lst += @bytes
      end
      lst += 1 unless idx.exclude_end?
      idx = idx.first
    elsif Fixnum === idx
      # Check type of cnt here and raise TypeError if can't coerce to int
      raise IndexError, "negative length #{cnt}" if cnt && cnt < 0
    elsif Regexp === idx
      cnt = 0 if cnt.nil?  # if capture group not specified default to 0
      m = idx.match(self)
      raise IndexError, "regexp not matched" if m.nil?
      raise IndexError, "index #{cnt} out of regexp" if cnt >= m.size
      idx = m.begin(cnt) 
      cnt = m.end(cnt) - idx
    elsif String === idx
      cnt = idx.length
      idx = index(idx)
      raise IndexError, "string not matched" if idx.nil?
    end

    if idx < 0
      idx += @bytes
    end

    if idx < 0 || idx >= @bytes
      raise IndexError, "index #{idx} out of string"
    end

    # Tidy-up the Range parameters
    if lst && cnt.nil?
      lst = idx if lst < idx
      cnt = lst - idx
    end

    if String === ent
      cnt = 1 if cnt.nil? # for index arg. only replace the indexed char
      out = ""
      out << substring(0, idx) if idx > 0
      out << ent
      out << substring(idx + cnt, @bytes - (idx + cnt)) if idx + cnt < @bytes
      replace out
    elsif Fixnum === ent
      @data.set_byte idx, ent
    end
    return ent
  end
  
  def oct
    self.to_i(8)
  end
  
  def hex
    self.to_i(16)
  end

  def to_f
    Ruby.primitive :string_to_f
  end
  
  def match(pattern)
    pattern = Regexp.new(pattern) unless Regexp === pattern
    pattern.match(self)
  end
end

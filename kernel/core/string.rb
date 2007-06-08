class String
  include Comparable, Enumerable
  
  alias_method :to_str, :to_s

  # Append --- Concatenates the given object to <i>self</i>. If the object is a
  # <code>Fixnum</code> between 0 and 255, it is converted to a character before
  # concatenation.
  #    
  #   a = "hello "
  #   a << "world"   #=> "hello world"
  #   a.concat(33)   #=> "hello world!"
  #---
  # NOTE: This overwrites String#<< defined in bootstrap
  #+++
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
    
    raise TypeError, "can't modify frozen string" if self.frozen?
    
    out = nil
    Ruby.asm "push other\npush self\nstring_append\nset out"
    return out
  end
  alias :concat :<<

  #---
  # NOTE: This overwrites String#dup defined in bootstrap
  #+++
  def dup
    out = nil
    Ruby.asm "push self\nstring_dup\nset out"
    out.taint if self.tainted?
    out.freeze if self.frozen?
    return out
  end

  # TODO: inspect is NOT dump!
  alias_method :dump, :inspect

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

  def initialize(arg)
    if Fixnum === arg
      @data = ByteArray.new(arg)
      @bytes = arg
      @characters = arg
      @encoding = nil
    else
      replace(arg.to_s)
    end
  end

  # call-seq:
  #   str % arg   => new_str
  #  
  # Format---Uses <i>str</i> as a format specification, and returns the result
  # of applying it to <i>arg</i>. If the format specification contains more than
  # one substitution, then <i>arg</i> must be an <code>Array</code> containing
  # the values to be substituted. See <code>Kernel::sprintf</code> for details
  # of the format string.
  # 
  #   "%05d" % 123                       #=> "00123"
  #   "%-5s: %08x" % [ "ID", self.id ]   #=> "ID   : 200e14d6"
  def %(arg)
    Sprintf::Parser.format(self, arg)
  end
  
  # call-seq:
  #   str * integer   => new_str
  #
  # Copy --- Returns a new <code>String</code> containing <i>integer</i> copies of
  # the receiver.
  #   
  #   "Ho! " * 3   #=> "Ho! Ho! Ho! "
  def *(num)
    str = []
    num.times { str << self }
    return str.join("")
  end
  
  # Replaces the contents and taintedness of <i>string</i> with the corresponding
  # values in <i>other</i>.
  # 
  #   s = "hello"         #=> "hello"
  #   s.replace "world"   #=> "world"
  def replace(other)
    # If we're replacing with ourselves, then we have nothing to do
    return self if self.equal?(other)

    other = other.coerce_string unless other.is_a? String

    raise TypeError, "can't modify frozen string" if self.frozen?

    @data = other.dup.data
    @bytes = other.bytes
    @characters = other.characters
    @encoding = other.encoding
    
    self.taint if other.tainted?
    
    self
  end
  alias :initialize_copy :replace
  
  # Replaces the contents and taintedness of <i>string</i> with the corresponding
  # values in <i>other</i> if they differ.
  # 
  #   s = "hello"           #=> "hello"
  #   s.replace_if "hello"  #=> nil
  #   s                     #=> "hello"
  #   s.replace_if "world"  #=> "world"
  #   s                     #=> "world"
  def replace_if(other)
    self == other ? nil : replace(other)
  end
  
  # Returns a new <code>String</code> with the given record separator removed
  # from the end of <i>str</i> (if present). If <code>$/</code> has not been
  # changed from the default Ruby record separator, then <code>chomp</code> also
  # removes carriage return characters (that is it will remove <code>\n</code>,
  # <code>\r</code>, and <code>\r\n</code>).
  #    
  #   "hello".chomp            #=> "hello"
  #   "hello\n".chomp          #=> "hello"
  #   "hello\r\n".chomp        #=> "hello"
  #   "hello\n\r".chomp        #=> "hello\n"
  #   "hello\r".chomp          #=> "hello"
  #   "hello \n there".chomp   #=> "hello \n there"
  #   "hello".chomp("llo")     #=> "he"
  def chomp(separator = $/)
    (str = self.dup).chomp!(separator) || str
  end
  
  # Modifies <i>self</i> in place as described for <code>String#chomp</code>,
  # returning <i>self</i>, or <code>nil</code> if no modifications were made.
  #---
  # NOTE: TypeError is raised in String#replace and not in String#chomp! when
  # self is frozen. This is intended behaviour.
  #+++
  def chomp!(separator = $/)
    return nil if separator.nil? || @bytes == 0

    length = @bytes - 1

    if separator == $/
      if @data[length] == ?\n
        length -= 1
        length -= 1 if length > 0 && @data[length] == ?\r
      elsif @data[length] == ?\r
        length -= 1
      else
        return nil
      end
      
      return replace(substring(0, length + 1))
    end
    
    separator = separator.coerce_string unless separator.is_a? String
    
    if separator.length == 0 
      while length > 0 && @data[length] == ?\n
        length -= 1
        length -= 1 if length > 0 && @data[length] == ?\r
      end
      
      return replace(substring(0, length + 1)) if length + 1 < @bytes
      return nil
    elsif separator.length > @bytes
      return nil
    elsif separator.length == 1 && separator == "\n"
      return self.chomp!
    elsif @data[length] == separator || self[-separator.length, separator.length] == separator
      return replace(substring(0, length + 1 - separator.length))
    end
  end
  
  # Returns a new <code>String</code> with the last character removed.  If the
  # string ends with <code>\r\n</code>, both characters are removed. Applying
  # <code>chop</code> to an empty string returns an empty
  # string. <code>String#chomp</code> is often a safer alternative, as it leaves
  # the string unchanged if it doesn't end in a record separator.
  #    
  #   "string\r\n".chop   #=> "string"
  #   "string\n\r".chop   #=> "string\n"
  #   "string\n".chop     #=> "string"
  #   "string".chop       #=> "strin"
  #   "x".chop.chop       #=> ""
  def chop
    (str = self.dup).chop! || str
  end
  
  def chop!
    return if @bytes == 0
    
    length = @bytes - 1
    length -= 1 if @data[length] == ?\n && @data[length - 1] == ?\r
    
    replace(substring(0, length))
  end
  
  # Returns a copy of <i>self</i> with the first character converted to uppercase
  # and the remainder to lowercase.
  # Note: case conversion is effective only in ASCII region.
  #    
  #   "hello".capitalize    #=> "Hello"
  #   "HELLO".capitalize    #=> "Hello"
  #   "123ABC".capitalize   #=> "123abc"
  def capitalize
    (str = self.dup).capitalize! || str
  end

  # Modifies <i>self</i> by converting the first character to uppercase and the
  # remainder to lowercase. Returns <code>nil</code> if no changes are made.
  # Note: case conversion is effective only in ASCII region.
  #    
  #   a = "hello"
  #   a.capitalize!   #=> "Hello"
  #   a               #=> "Hello"
  #   a.capitalize!   #=> nil
  def capitalize!
    return if @bytes == 0
    raise TypeError, "can't modify frozen string" if self.frozen?
    
    modified = false
    
    if @data[0].islower
      @data[0] = @data[0].toupper
      modified = true
    end
    
    1.upto(@bytes - 1) do |i|
      if @data[i].isupper
        @data[i] = @data[i].tolower
        modified = true
      end
    end
  
    modified ? self : nil
  end
  
  def swapcase
    (str = self.dup).swapcase! || str
  end
  
  def swapcase!
    return if @bytes == 0
    raise TypeError, "can't modify frozen string" if self.frozen?
  
    modified = false
  
    @bytes.times do |i|
      if @data[i].islower
        @data[i] = @data[i].toupper
        modified = true
      elsif @data[i].isupper
        @data[i] = @data[i].tolower
        modified = true
      end
    end
  
    modified ? self : nil
  end
  
  def upcase
    (str = self.dup).upcase! || str
  end
  
  def upcase!
    return if @bytes == 0
    raise TypeError, "can't modify frozen string" if self.frozen?
  
    modified = false
  
    @bytes.times do |i|
      if @data[i].islower
        @data[i] = @data[i].toupper
        modified = true
      end
    end

    modified ? self : nil
  end
  
  def downcase
    (str = self.dup).downcase! || str
  end
  
  def downcase!
    return if @bytes == 0
    raise TypeError, "can't modify frozen string" if self.frozen?
  
    raise Type
  
    modified = false
  
    @bytes.times do |i|
      if @data[i].isupper
        @data[i] = @data[i].tolower
        modified = true
      end
    end

    modified ? self : nil
  end

  def reverse
    self.dup.reverse!
  end

  def reverse!
    return self if @bytes <= 1
    raise TypeError, "can't modify frozen string" if self.frozen?
    
    i = 0
    j = @bytes - 1
    while i < j
      a = @data[i]
      b = @data[j]
      @data[j] = a
      @data[i] = b
      i += 1
      j -= 1
    end
    self
  end
  
  def lstrip
    (str = self.dup).lstrip! || str
  end
  
  def lstrip!
    return if @bytes == 0
  
    start = 0
    each_byte do |c|
      if c.isspace or c == 0
        start += 1
      else
        break
      end
    end
  
    return if start == 0
    replace(substring(start, @bytes - start))
  end
  
  def rstrip
    (str = self.dup).rstrip! || str
  end
  
  def rstrip!
    return if @bytes == 0
    
    stop = @bytes - 1
    while stop >= 0
      c = @data[stop]
      if c.isspace || c == 0
        stop -= 1
      else
        break
      end
    end

    return if stop + 1 == @bytes
    replace(substring(0, stop + 1))
  end

  def strip
    (str = self.dup).strip! || str
  end
  
  def strip!
    left = lstrip!
    right = rstrip!
    left.nil? && right.nil? ? nil : self
  end

  def gsub(pattern, rep=nil)
    str = self.dup
    out = ""
    pattern = Regexp.new(pattern) if String === pattern

    if block_given?
      while m = pattern.match(str)
        out << m.pre_match #str[0...m.begin(0)] if m.begin(0) > 0
        out << yield(m[0])
        str = m.post_match #str[m.end(0)..-1]
      end
    else
      raise ArgumentError, "wrong number of arguments (1 for 2)" if rep == nil
      while m = pattern.match(str)
        out << m.pre_match #str[0...m.begin(0)] if m.begin(0) > 0
        out << rep.gsub(/\\\d/) { |x| m[x[0] - ?0] }
        str = m.post_match #str[m.end(0)..-1]
      end
    end
    return out << str
  end

  def gsub!(pattern, rep=nil, &block)
    replace_if(gsub(pattern, rep, &block))
  end

  def sub(pattern, rep=nil)
    str = self.dup
    out = ""
    pattern = Regexp.new(pattern) if String === pattern
    m = pattern.match(str)
    return self.dup if m == nil
    out << m.pre_match
    if block_given?
      out << yield(m[0])
    else
      raise ArgumentError, "wrong number of arguments (1 for 2)" if rep == nil
      out << rep.gsub(/\\\d/) { |x| m[x[0] - ?0] }
    end
    return out << m.post_match
  end

  def sub!(pattern, rep=nil, &block)
    replace_if(sub(pattern, rep, &block))
  end

  def insert(idx, str)
    if idx < 0
      idx += length + 1
    end
    raise IndexError, "index #{idx} out of string" if idx < 0 || idx > length
    if idx < length
      self[idx,0] = str
    else
      self << str
    end
    self
  end

  def succ
    return "" if length == 0
    out = self.dup

    start = length-1
    while start >= 0       # can't break from a step or downto yet
      if out[start].isalnum
        break
      else
        start -= 1
      end
    end
    start = length-1 if start < 0
    
    carry = false
    c = 0
    start.step(0, -1) do |idx|
      c = out[idx]
      carry = true 
      if c == ?9
        c = ?0
      elsif c == ?Z
        c = ?A
      elsif c == ?z
        c = ?a
      else
        c = (c + 1) % 256
        carry = false if c != 0
      end
      out[idx] = c
      return out if !carry
    end
    c += 1 if c == ?0 || c == 0
    # work around for << not taking Fixnum
    out = " " << out
    out[0] = c
    return out
  end

  def succ!
    replace_if(succ)
  end

  alias_method :next, :succ
  alias_method :next!, :succ!

  def expand_tr_str(str)
    out = ""
    str.gsub(/[^-]-[^-]/) { |r| out = "" ; r[0].upto(r[2]) { |c| out << c }; out }
  end

  def tr(from_str, to_str)
    tr_string(from_str, to_str, false)
  end

  def tr!(from_str, to_str)
    replace_if(tr_string(from_str, to_str, false))
  end

  def tr_s(from_str, to_str)
    tr_string(from_str, to_str, true)
  end

  def tr_s!(from_str, to_str)
    replace_if(tr_string(from_str, to_str, true))
  end

  # used by count, delete, squeeze
  def intersect_string_from_arg(*arg)
    raise ArgumentError, "wrong number of arguments" if arg.length == 0
    raise TypeError, "can't convert #{arg[0].class} to String" unless String === arg[0]
    first = expand_tr_str(arg[0])
    if arg.size > 1
      (1...arg.size).each do |arg_idx|
        raise TypeError, "can't convert #{arg[arg_idx].class} to String" unless String === arg[arg_idx]
        second = expand_tr_str(arg[arg_idx])
        str = ""
        remove_flag = second.data[0] == ?^
        (0...first.length).each do |idx|
          pos = second.index(first.data[idx])
          if remove_flag == true
            str << first.data[idx] if pos == nil
          else
            str << first.data[idx] if pos != nil
          end
        end
        first = str
        return nil if first.length == 0
      end
    end
    first 
  end

  def delete(*arg)
    str = intersect_string_from_arg(*arg)
    return self.dup if str == nil
    tr_string(str,"",false)
  end

  def delete!(*str)
    replace_if(delete(*str))
  end

  def count(*arg)
    str = intersect_string_from_arg(*arg)
    return 0 if str == nil
    char_map = 0.chr * 256
    str.each_byte { |c| char_map[c] = 1 }
    cnt = 0
    each_byte do |c|
      cnt +=1 if char_map[c] == 1
    end
    cnt
  end

  def squeeze(*arg)

    out = ""
    last_char = -1

    if arg.length > 0
      # Build the target character map
      str = intersect_string_from_arg(*arg)
      return self.dup if str == nil
      char_map = 0.chr * 256
      str.each_byte { |c| char_map[c] = 1 }

      each_byte do |c|
        if char_map[c] == 1
          out << c if c != last_char
        else
          out << c
        end
        last_char = c
      end
    else
      each_byte do |c|
        out << c if c != last_char
        last_char = c
      end
    end
    out
  end

  def squeeze!(*arg)
    replace_if(squeeze(*arg))
  end

  # Generic function for the family of tr functions
  def tr_string(from_str, to_str, no_dups=false)
    raise TypeError, "can't convert #{from_str.class} to String" unless String === from_str
    raise TypeError, "can't convert #{to_str.class} to String"   unless String === to_str

    return "" if from_str == ""

    del_chars = to_str.length == 0
    from_str  = expand_tr_str(from_str)
    to_str    = expand_tr_str(to_str)

    # Build out the to_str translations to the same length as from_str
    if to_str.length < from_str.length
      to_str << ((to_str.length > 0 ? to_str[-1,1] : ' ') * (from_str.length - to_str.length))
    end

    # Create an ASCII  translation map
    trans = 1.chr * 256
    c = 0
    if from_str[0] == ?^
      # This is the inverse map
      cnt = 0
      (1...from_str.length).each do |idx| 
        trans.data[from_str.data[idx]] = 0
      end
      (0..255).each do |idx|
        c = trans.data[idx] == 1 ? to_str.data[cnt] : idx
        trans.data[idx] = c
        cnt += 1 if cnt < (to_str.length-1)
      end
    else
      (0..255).each do |idx|
        trans[idx] = idx
      end
      (0...from_str.length).each do |idx| 
        trans.data[from_str.data[idx]] = to_str.data[idx]
      end
    end

    # Translate self using the trans character map
    out = self.dup
    idx = 0
    w_idx = 0
    last_char = -1
    while idx < out.length
      c = trans[out.data[idx]]
      no_trans_flag = c == out.data[idx]
      if no_dups == false || no_trans_flag == true || c != last_char
        # If a translation occurred remember the last char to remove
        # duplicate translations with the no_dup flag (if required).
        # Ordering is important here don't move this test below the
        # substitution.
        last_char = no_trans_flag == true ? -1 : c
        if del_chars == false || no_trans_flag == true
          out.data[w_idx] = c
          w_idx += 1
        end
      end
      idx += 1
    end
    # truncate the string if required
    out[0,w_idx]
  end

  def tr!(from_str, to_str)
    replace_if(tr(from_str, to_str))
  end

  # Match --- If <i>pattern</i> is a <code>Regexp</code>, use it as a pattern to match
  # against <i>self</i>, and return the position the match starts, or 
  # <code>nil</code> if there is no match. Otherwise, invoke
  # <i>pattern.=~</i>, passing <i>self</i> as an argument.
  # 
  # The default <code>=~</code> in <code>Object</code> returns <code>false</code>.
  #    
  #   "cat o' 9 tails" =~ /\d/ #=> 7
  #   "cat o' 9 tails" =~ 9    #=> false
  def =~(pattern)
    case pattern
    when Regexp
      pattern.match(self)
    when String
      raise TypeError, "type mismatch: String given"
    else
      pattern =~ self
    end
  end
  
  def include?(needle)
    if needle.is_a? Fixnum
      each_byte { |b| return true if b == arg }
      return false
    end

    if needle.respond_to? :to_str
      return !self.index(needle.to_str).nil?
    else
      raise TypeError, "can't convert #{needle.class} into String"
    end
  end

  def index(needle, offset = 0)
    offset = @bytes + offset if offset < 0
    return nil if offset < 0 || offset > @bytes

    # What are we searching for?
    case needle
    when Fixnum
      (offset...self.size).each do |i|
        return i if @data[i] == needle
      end
    when String
      needle_size = needle.size
      return if needle_size <= 0
      
      max = @bytes - needle_size
      return if max < 0 # <= 0 maybe?
      
      offset.upto(max) do |i|
        if @data[i] == needle.data[0]
          return i if substring(i, needle_size) == needle
        end
      end
    when Regexp
      if match = needle.match(self[offset..-1])
        return (offset + match.begin(0))
      end
    else
      raise TypeError, "type mismatch: #{needle.class} given"
    end

    return nil
  end

  def rindex(arg, finish = nil )
    if finish
      raise TypeError, "can't convert #{finish.class} into Integer" if !finish.is_a?(Integer)
      finish += @bytes if finish < 0
      return nil if finish < 0
      finish = @bytes - 1 if finish >= @bytes
    else
      finish = @bytes - 1
    end
    
    if arg.is_a?(Fixnum)
      finish.step(0, -1) do |idx|
        return idx if @data[idx] == arg
      end
    elsif arg.is_a? String
      return nil if arg.length > finish
      len   = arg.length
      start = finish - len
      start.step(0, -1) do |idx|
        if @data[idx] == arg.data[0]
          return idx if substring(idx,len) == arg
        end
      end
    elsif arg.is_a? Regexp
      mstr = self[0..finish]
      offset = nil
      while m = arg.match(mstr)
        offset = offset ? offset += m.begin(0) + len : m.begin(0)
        len = m.end(0) - m.begin(0)
        mstr = m.post_match
      end
      return offset
    else
      raise ArgumentError.new("String#index cannot accept #{arg.class} objects")
    end
    return nil
  end

  # justify left = -1, center = 0, right = 1
  def justify_string(width, str, justify)
    raise TypeError, "can't convert #{width.class} into Integer" if !width.is_a?(Integer)
    raise TypeError, "can't convert #{str.class} into String" if !str.respond_to?(:to_str)
    return self if width <= @bytes
    pad = width - @bytes
    out = str.to_str * (pad / str.length)
    out << str[0, pad - out.length] if out.length < pad
    # Left justification
    return self << out if justify == -1
    # Right justification
    return out << self if justify == 1
    # and finially center
    split = (width / 2) - (@bytes / 2)
    return out.insert(split, self)
  end

  def rjust(width, str=" ")
    justify_string(width, str, 1)
  end

  def ljust(width, str=" ")
    justify_string(width, str, -1)
  end

  def center(width, str=" ")
    justify_string(width, str, 0) 
  end

  def [](*args)
    if args.size == 2
      case args.first
      when Regexp
        match = args.first.match(self)
        return match ? match[args.last] : nil
      else
        start, count = *args

        start = @bytes + start if start < 0
        count = @bytes - start if start + count > @bytes

        return "" if count == 0
        return nil if start < 0 || start > @bytes
        
        return substring(start, count)
      end
    elsif args.size == 1
      case args.first
      when Fixnum
        index = args.first
        index += @bytes if index < 0
        if 0 <= index && index < @bytes
          return @data[index]
        end
      when Regexp
        return self[args.first, 0]
      when String
        return self.include?(args.first) ? args.first.dup : nil
      when Range
        range  = args.first
        
        start   = range.first
        length  = range.last

        start += @bytes if start < 0

        length += @bytes if length < 0
        length += 1 unless range.exclude_end?
        
        return "" if start == @bytes
        return nil if start > @bytes
        
        length = @bytes if length > @bytes
        length = length - start
        length = 0 if length < 0
        
        return self[start, length]
      end
    else
      raise ArgumentError, "wrong number of arguments (#{args.size} for 1)"
    end
    
    return nil
  end
  alias_method :slice, :[]

  def slice!(*args)
    result = slice(*args)
    self[*args] = '' unless result.nil?
    result
  end

  def []=(*args)
    if args.size == 3
    
      case args.first
      when Regexp
        # Regexp match with given index
        index = args[1]
        match = args.first.match(self)
        
        raise IndexError, "regexp not matched" unless match
        raise IndexError, "index #{index} out of regexp" if index.abs >= match.size
        
        # Negative index?
        index += match.size if index < 0

        # Let's hope this is not evil...
        start  = match.begin(index)
        length = match.end(index) - start 
        self[start, length] = args.last
      when Fixnum
        start, count, content = *args
        start = @bytes + start if start < 0
        
        if content.is_a?(Fixnum)
          @data[start] = content
        else
          raise IndexError, "negative length #{count}" if count < 0

          output = ""
          output << substring(0, start) if start != 0
          output << content
          output << substring(start + count, @bytes - (start + count)) if start + count < @bytes
          
          replace(output)
        end

        return content
      end
      
    elsif args.size == 2

      case args.first
      when Regexp
        self[args.first, 0] = args.last
      when Fixnum
        self[args.first, 1] = args.last
      when Range
        range  = args.first
        
        start   = range.first
        length  = range.last

        start += @bytes if start < 0

        length = @bytes if length > @bytes
        length += @bytes if length < 0
        length += 1 unless range.exclude_end?
        
        length = length - start
        length = 0 if length < 0
        
        self[start, length] = args.last
      when String
        length = args.first.length
        start  = index(args.first)
        
        raise IndexError, "string not matched" unless start
        self[start, length] = args.last
      end
    
    else
      raise ArgumentError, "wrong number of arguments (#{args.size} for 2)" 
    end
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
  
  def sum(bits=16)
    sum = 0
    each_byte do |b|
      sum += b
    end
    sum & ((1 << bits) - 1)
  end
  
  def upto(stop)
    unless String === stop
      if stop.respond_to?(:to_str)
        stop = stop.to_str
      else
        raise TypeError, "can't convert #{stop.class} to String"
      end
    end

    raise LocalJumpError, "no block given" unless block_given?

    if self > stop
      return self
    end

    str = self.dup
    loop do
      yield str.dup
      str.succ!
      break if str.size > stop.size || str > stop
    end
    self
  end

  def casecmp(to)
    self.upcase <=> to.upcase
  end

  def each(separator=$/)
    raise LocalJumpError, "no block given" unless block_given?
    index      = separator.length
    min_index  = 0
    last_index = 0
    
    while index < self.length
      min_index = index - separator.length + 1

      if self[min_index..index] == separator
        yield self[last_index..index]
        last_index = index + 1
      end
      index += 1
    end

    unless last_index == self.length
      yield self[last_index..index]
    end

    self
  end
  alias_method :each_line, :each


  def scan(pattern, &block)
    pattern = Regexp.new(pattern) if String === pattern
    index = 0
    if block_given?
      ret = self
    else
      ret = []
      block = lambda{|x| ret << x}
    end

    while index < self.length and md = pattern.match(self[index..-1])
      block.call(md.length > 1 ? md.captures : md[0])
      index = index + md.end(md.length-1)
    end
    return ret
  end

  def crypt(other_str)
    Ruby.primitive :str_crypt
    crypt(other_str.coerce_string) unless String === other_str
    raise ArgumentError.new("salt must be at least 2 characters") if other_str.size < 2
  end
  
  alias :eql? :==

=begin

  # Should be added when Crypt is required
  def crypt(other_str)
    raise NotImplementedError
  end


  def unpack(format) # => anArray
    raise NotImplementedError
  end
=end

end
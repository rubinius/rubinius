# depends on: comparable.rb enumerable.rb
  
# Default Ruby Record Separator
# Used in this file and by various methods that need to ignore $/
DEFAULT_RECORD_SEPARATOR = "\n"

class String
  include Comparable
  include Enumerable
  
  ivar_as_index :bytes => 0, :characters => 1, :encoding => 2, :data => 3, :hash => 4, :shared => 5
  def bytes     ; @bytes      ; end
  def characters; @characters ; end
  def encoding  ; @encoding   ; end
  def data      ; @data       ; end
  def __ivars__ ; nil         ; end

  def initialize(arg = "")
    if Fixnum === arg
      # + 1 for the null on the end.
      @data = ByteArray.new(arg+1)
      @bytes = arg
      @characters = arg
      @encoding = nil
    else
      replace(arg)
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
    # raise ArgumentError, "Too many arguments for format string" if $DEBUG && self.scan(/%/).size < arg.size
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
    unless num.is_a? Integer
      raise "Can't convert #{num.class} to Integer" unless num.respond_to? :to_int
      num = num.to_int
    end
    
    raise RangeError, "bignum too big to convert into `long'" if num.is_a? Bignum
    raise ArgumentError, "unable to multiple negative times (#{num})" if num < 0

    str = []
    num.times { str << self }
    str = self.class.new(str.join)
    str.taint if self.tainted?
    return str
  end
  
  # Concatenation --- Returns a new <code>String</code> containing
  # <i>other</i> concatenated to <i>string</i>.
  # 
  #   "Hello from " + self.to_s   #=> "Hello from main"
  def +(other)
    r = String.new(self) << other
    r.taint if self.tainted? || other.tainted?
    r
  end

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
      else
        other = other.coerce_to(String, :to_str)
      end
    end
    
    self.modify!
    self.taint if other.tainted?
    self.append(other)
  end
  alias_method :concat, :<<

  # call-seq:
  #   str <=> other_str   => -1, 0, +1
  # 
  # Comparison --- Returns -1 if <i>other_str</i> is less than, 0 if
  # <i>other_str</i> is equal to, and +1 if <i>other_str</i> is greater than
  # <i>str</i>. If the strings are of different lengths, and the strings are
  # equal when compared up to the shortest length, then the longer string is
  # considered greater than the shorter one. If the variable <code>$=</code> is
  # <code>false</code>, the comparison is based on comparing the binary values
  # of each character in the string. In older versions of Ruby, setting
  # <code>$=</code> allowed case-insensitive comparisons; this is now deprecated
  # in favor of using <code>String#casecmp</code>.
  #
  # <code><=></code> is the basis for the methods <code><</code>,
  # <code><=</code>, <code>></code>, <code>>=</code>, and <code>between?</code>,
  # included from module <code>Comparable</code>. The method
  # <code>String#==</code> does not use <code>Comparable#==</code>.
  #    
  #    "abcdef" <=> "abcde"     #=> 1
  #    "abcdef" <=> "abcdef"    #=> 0
  #    "abcdef" <=> "abcdefg"   #=> -1
  #    "abcdef" <=> "ABCDEF"    #=> 1
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
      return unless tmp = (other <=> self)
      return -tmp # We're not supposed to convert to integer here
    end
  end

  # call-seq:
  #    str == obj   => true or false
  # 
  # Equality---If <i>obj</i> is not a <code>String</code>, returns
  # <code>false</code>. Otherwise, returns <code>true</code> if <i>str</i>
  # <code><=></code> <i>obj</i> returns zero.
  #---
  # TODO: MRI does simply use <=> for Strings here, so what's this code about?
  #+++ 
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
  alias_method :===, :==

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

  # call-seq:
  #    str[fixnum]                 => fixnum or nil
  #    str[fixnum, fixnum]         => new_str or nil
  #    str[range]                  => new_str or nil
  #    str[regexp]                 => new_str or nil
  #    str[regexp, fixnum]         => new_str or nil
  #    str[other_str]              => new_str or nil
  #    str.slice(fixnum)           => fixnum or nil
  #    str.slice(fixnum, fixnum)   => new_str or nil
  #    str.slice(range)            => new_str or nil
  #    str.slice(regexp)           => new_str or nil
  #    str.slice(regexp, fixnum)   => new_str or nil
  #    str.slice(other_str)        => new_str or nil
  # 
  # Element Reference --- If passed a single <code>Fixnum</code>, returns the code
  # of the character at that position. If passed two <code>Fixnum</code>
  # objects, returns a substring starting at the offset given by the first, and
  # a length given by the second. If given a range, a substring containing
  # characters at offsets given by the range is returned. In all three cases, if
  # an offset is negative, it is counted from the end of <i>str</i>. Returns
  # <code>nil</code> if the initial offset falls outside the string, the length
  # is negative, or the beginning of the range is greater than the end.
  #    
  # If a <code>Regexp</code> is supplied, the matching portion of <i>str</i> is
  # returned. If a numeric parameter follows the regular expression, that
  # component of the <code>MatchData</code> is returned instead. If a
  # <code>String</code> is given, that string is returned if it occurs in
  # <i>str</i>. In both cases, <code>nil</code> is returned if there is no
  # match.
  #    
  #    a = "hello there"
  #    a[1]                   #=> 101
  #    a[1,3]                 #=> "ell"
  #    a[1..3]                #=> "ell"
  #    a[-3,2]                #=> "er"
  #    a[-4..-2]              #=> "her"
  #    a[12..-1]              #=> nil
  #    a[-2..-4]              #=> ""
  #    a[/[aeiou](.)\1/]      #=> "ell"
  #    a[/[aeiou](.)\1/, 0]   #=> "ell"
  #    a[/[aeiou](.)\1/, 1]   #=> "l"
  #    a[/[aeiou](.)\1/, 2]   #=> nil
  #    a["lo"]                #=> "lo"
  #    a["bye"]               #=> nil
  def [](*args)
    if args.size == 2
      if args.first.is_a? Regexp
        return self.subpattern(args[0], args[1].coerce_to(Integer, :to_int))
      else
        return self.substring(args[0].coerce_to(Integer, :to_int), args[1].coerce_to(Integer, :to_int))
      end
    elsif args.size != 1
      raise ArgumentError, "wrong number of arguments (#{args.size} for 1)"
    end

    case index = args.first
    when Regexp
      ret = self.subpattern(index, 0)
      self.match(index)
      return ret
    when String
      return self.include?(index) ? index.dup : nil
    when Range
      start   = index.first.coerce_to(Integer, :to_int)
      length  = index.last.coerce_to(Integer, :to_int)

      start += @bytes if start < 0

      length += @bytes if length < 0
      length += 1 unless index.exclude_end?
      
      return "" if start == @bytes
      return nil if start < 0 || start > @bytes
      
      length = @bytes if length > @bytes
      length = length - start
      length = 0 if length < 0
      
      return self.substring(start, length)
    else
      index = index.coerce_to(Integer, :to_int)
      index = @bytes + index if index < 0
      return if index < 0 || @bytes <= index
      return @data[index]
    end
  end
  alias_method :slice, :[]

  # call-seq:
  #   str[fixnum] = fixnum
  #   str[fixnum] = new_str
  #   str[fixnum, fixnum] = new_str
  #   str[range] = aString
  #   str[regexp] = new_str
  #   str[regexp, fixnum] = new_str
  #   str[other_str] = new_str
  # 
  # Element Assignment --- Replaces some or all of the content of <i>str</i>. The
  # portion of the string affected is determined using the same criteria as
  # <code>String#[]</code>. If the replacement string is not the same length as
  # the text it is replacing, the string will be adjusted accordingly. If the
  # regular expression or string is used as the index doesn't match a position
  # in the string, <code>IndexError</code> is raised. If the regular expression
  # form is used, the optional second <code>Fixnum</code> allows you to specify
  # which portion of the match to replace (effectively using the
  # <code>MatchData</code> indexing rules. The forms that take a
  # <code>Fixnum</code> will raise an <code>IndexError</code> if the value is
  # out of range; the <code>Range</code> form will raise a
  # <code>RangeError</code>, and the <code>Regexp</code> and <code>String</code>
  # forms will silently ignore the assignment.
  def []=(*args)
    if args.size == 3
      if args.first.is_a? Regexp
        self.subpattern_set(args[0], args[1].coerce_to(Integer, :to_int), args[2])
      else
        self.splice(args[0].coerce_to(Integer, :to_int), args[1].coerce_to(Integer, :to_int), args[2])
      end
      
      return args.last
    elsif args.size != 2
      raise ArgumentError, "wrong number of arguments (#{args.size} for 2)"
    end
    
    index = args[0]
    replacement = args[1]
    
    case index
    when Regexp
      self.subpattern_set(index, 0, replacement)
    when String
      unless start = self.index(index)
        raise IndexError, "string not matched"
      end
      
      self.splice(start, index.length, replacement)
    when Range
      start   = index.first.coerce_to(Integer, :to_int)
      length  = index.last.coerce_to(Integer, :to_int)

      start += @bytes if start < 0
      
      # TODO: this is wrong
      return nil if start < 0 || start > @bytes

      length = @bytes if length > @bytes
      length += @bytes if length < 0
      length += 1 unless index.exclude_end?
      
      length = length - start
      length = 0 if length < 0
      
      self.splice(start, length, replacement)
    else
      index = index.coerce_to(Integer, :to_int)
      raise IndexError, "index #{index} out of string" if @bytes <= index
      
      if index < 0
        raise IndexError, "index #{index} out of string" if -index > @bytes
        index += @bytes
      end
      
      if replacement.is_a?(Fixnum)
        self.modify!
        @data[index] = replacement
      else
        self.splice(index, 1, replacement)
      end
    end
    return replacement
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
    self.modify!
    return if @bytes == 0
    
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

  # Case-insensitive version of <code>String#<=></code>.
  #    
  #   "abcdef".casecmp("abcde")     #=> 1
  #   "aBcDeF".casecmp("abcdef")    #=> 0
  #   "abcdef".casecmp("abcdefg")   #=> -1
  #   "abcdef".casecmp("ABCDEF")    #=> 0
  def casecmp(to)
    self.upcase <=> to.upcase
  end

  # If <i>integer</i> is greater than the length of <i>str</i>, returns a new
  # <code>String</code> of length <i>integer</i> with <i>str</i> centered and
  # padded with <i>padstr</i>; otherwise, returns <i>str</i>.
  #    
  #    "hello".center(4)         #=> "hello"
  #    "hello".center(20)        #=> "       hello        "
  #    "hello".center(20, '123') #=> "1231231hello12312312"
  def center(integer, padstr = " ")
    justify(integer, :center, padstr)
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

    if separator == $/ && separator == DEFAULT_RECORD_SEPARATOR
      return smart_chomp!
    end
    
    separator = StringValue(separator)
    length = @bytes - 1
    
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
      return self.smart_chomp!
    elsif @data[length].chr == separator || self[-separator.length, separator.length] == separator
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

  # Processes <i>str</i> as for <code>String#chop</code>, returning <i>str</i>,
  # or <code>nil</code> if <i>str</i> is the empty string.  See also
  # <code>String#chomp!</code>.
  def chop!
    return if @bytes == 0
    
    length = @bytes - 1
    length -= 1 if @data[length] == ?\n && @data[length - 1] == ?\r
    
    replace(substring(0, length))
  end
  
  # Each <i>other_string</i> parameter defines a set of characters to count.  The
  # intersection of these sets defines the characters to count in
  # <i>self</i>. Any <i>other_str</i> that starts with a caret (^) is
  # negated. The sequence c1--c2 means all characters between c1 and c2.
  #    
  #   a = "hello world"
  #   a.count "lo"            #=> 5
  #   a.count "lo", "o"       #=> 2
  #   a.count "hello", "^l"   #=> 4
  #   a.count "ej-m"          #=> 4
  def count(*strings)
    raise ArgumentError, "wrong number of Arguments" if strings.empty?
    return 0 if @bytes == 0
    
    table = setup_tr_table(*strings)
    
    count = 0
    self.each_byte { |c| count += 1 if table[c] }
    count
  end

  # Applies a one-way cryptographic hash to <i>self</i> by invoking the standard
  # library function <code>crypt</code>. The argument is the salt string, which
  # should be two characters long, each character drawn from
  # <code>[a-zA-Z0-9./]</code>.
  def crypt(other_str)
    other_str = StringValue(other_str)
    raise ArgumentError.new("salt must be at least 2 characters") if other_str.size < 2

    hash = __crypt__(other_str)
    hash.taint if self.tainted? || other_str.tainted?
    hash
  end

  # Returns a copy of <i>self</i> with all characters in the intersection of its
  # arguments deleted. Uses the same rules for building the set of characters as
  # <code>String#count</code>.
  #    
  #   "hello".delete "l","lo"        #=> "heo"
  #   "hello".delete "lo"            #=> "he"
  #   "hello".delete "aeiou", "^e"   #=> "hell"
  #   "hello".delete "ej-m"          #=> "ho"
  def delete(*args)
    (str = self.dup).delete!(*args) || str
  end

  # Performs a <code>delete</code> operation in place, returning <i>self</i>, or
  # <code>nil</code> if <i>self</i> was not modified.
  def delete!(*strings)
    raise ArgumentError, "wrong number of arguments" if strings.empty?
    
    # We need to raise a TypeError here, event if we won't be making any changes.
    raise TypeError, "can't modify frozen string" if self.frozen?
    
    return if @bytes == 0
    
    table = setup_tr_table(*strings)
    
    # TODO: Can't we use the ByteArray directly here?
    new = []
    self.each_byte do |c|
      new << c.chr unless table[c]
    end
    new = new.join
    new != self ? replace(new) : nil # TODO
  end
  
  # Returns a copy of <i>str</i> with all uppercase letters replaced with their
  # lowercase counterparts. The operation is locale insensitive---only
  # characters ``A'' to ``Z'' are affected.
  # 
  # "hEllO".downcase   #=> "hello"
  def downcase
    (str = self.dup).downcase! || str
  end
  
  # Downcases the contents of <i>str</i>, returning <code>nil</code> if no
  # changes were made.
  def downcase!
    return if @bytes == 0
    self.modify!
  
    modified = false
  
    @bytes.times do |i|
      if @data[i].isupper
        @data[i] = @data[i].tolower
        modified = true
      end
    end

    modified ? self : nil
  end
  
  
  
  def setup_tr_table(*strings)
    table = Array.new(256, true)
    
    strings.each do |str|
      str = str.coerce_to(String, :to_str) unless str.is_a? String
      str = str.to_expanded_tr_string
      
      if str.length > 1 && str[0] == ?^
        flag, start = true, 1 
      else
        flag, start = false, 0
      end
      
      buf = Array.new(256, flag)

      (start...str.size).each do |i|
        c = str[i]
        buf[c] = !flag
      end
      
      table.each_index do |i|
        table[i] = table[i] && buf[i]
      end
    end
    
    table
  end

  def to_expanded_tr_string
    self.reverse.gsub(/.-./) { |r| (r[2]..r[0]).to_a.map { |c| c.chr } }
  end
  
  def smart_chomp!
    length = @bytes - 1
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

  def justify(integer, direction, padstr = " ")
    integer = integer.coerce_to(Integer, :to_int) unless integer.is_a?(Fixnum)
    padstr = padstr.coerce_to(String, :to_str) unless padstr.is_a?(String)
    
    raise ArgumentError, "zero width padding" if padstr.length == 0

    padsize = integer - self.size
    padsize = padsize > 0 ? padsize : 0
    case direction
    when :right
      dup.insert(0, padstr.to_padding(padsize))
    when :left
      dup.insert(-1, padstr.to_padding(padsize))
    when :center
      lpad = padstr.to_padding((padsize / 2.0).floor)
      rpad = padstr.to_padding((padsize / 2.0).ceil)
      dup.insert(0, lpad).insert(-1, rpad)
    end
  end
  
  # Convert this string to a padstring for use in String#justify.
  def to_padding(padsize)
    if padsize != 0
      (self * ((padsize / self.size) + 1)).slice(0, padsize)
    else
      ""
    end
  end

  # Raises a TypeError on frozen strings and unshares shared ones.
  def modify!
    raise TypeError, "can't modify frozen string" if self.frozen?
    
    if @shared
      @data = @data.dup
      @shared = nil
    end
  end

  def subpattern(pattern, capture)
    # TODO: A part of the functionality here should go into MatchData#[]
    return if (match = pattern.match(self)).nil? || capture >= match.size
    if capture < 0
      capture += match.size
      return if capture <= 0
    end
    
    start = match.begin(capture)
    count = match.end(capture) - match.begin(capture)
    str = self.substring(start, count)
    str.taint if pattern.tainted?
    str
  end

  def subpattern_set(pattern, capture, replacement)
    unless match = pattern.match(self)
      raise IndexError, "regexp not matched"
    end
    
    raise IndexError, "index #{index} out of regexp" if capture >= match.size
    
    if capture < 0
      raise IndexError, "index #{index} out of regexp" if -capture >= match.size
      capture += match.size
    end
    
    start  = match.begin(capture)
    length = match.end(capture) - start 
    self.splice(start, length, replacement)
  end

  def substring(start, count)
    return if count < 0 || start > @bytes || -start > @bytes
    
    start += @bytes if start < 0
    
    count = @bytes - start if start + count > @bytes
    count = 0 if count < 0

    str = self.class.allocate
    str.taint if self.tainted?
    str.put 0, count
    str.put 1, count
    if count == 0
      str.put(3, ByteArray.new(0))
    else
      str.put(3, @data.fetch_bytes(start, count))
    end
    
    return str
  end

  def splice(start, count, replacement)
    raise IndexError, "negative length #{count}" if count < 0
    self.modify!
    raise IndexError, "index #{start} out of string" if @bytes < start || -start > @bytes
    
    start += @bytes if start < 0
    count = @bytes - start if start + count > @bytes
    
    # TODO: Optimize this by using the @data ByteArray directly?
    output = ""
    output << substring(0, start) if start != 0
    output << replacement.coerce_to(String, :to_str)
    output << substring(start + count, @bytes - (start + count)) if start + count < @bytes
      
    replace(output)
  end

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
  alias_method :size, :length
  
  alias_method :to_str, :to_s

  #---
  # NOTE: This overwrites String#dup defined in bootstrap.
  # TODO: Remove me and make string_dup check taint and freeze.
  # TODO: Make string_dup compatible with String subclasses
  #+++
  def dup
    if self.instance_of? String
      out = Ruby.asm "push self\nstring_dup\n"
    else
      out = self.class.new(self)
    end
    
    out.taint if self.tainted?
    out.freeze if self.frozen?
    return out
  end

  # FIXME - Make Unicode-safe
  def codepoints
    chars = []
    @bytes.times do |pos|
      chars << self.substring(pos, 1)
    end
    chars
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
    raise ArgumentError, "interning empty string" if self.empty?
    __symbol_lookup__
  end
  alias_method :intern, :to_sym

  def each_byte(&prc)
    i = 0
    while i < @bytes do
      yield @data.get_byte(i)
      i += 1
    end
    self
  end
  
  def empty?
    @bytes == 0
  end
  
  def split(pattern = nil, limit = nil)
    limit = limit.to_int if (!limit.is_a?(Integer) && limit.respond_to?(:to_int))
    return [self.dup] if limit == 1
    limited = limit.to_i > 1
    pattern ||= ($; || " ")
    
    spaces = true if pattern == ' '
    pattern = /\s+/ if pattern == nil || pattern == ' '
    pattern = pattern.to_str if ![String, Regexp].include?(pattern.class) && pattern.respond_to?(:to_str)
    pattern = Regexp.new(Regexp.quote(pattern)) unless Regexp === pattern
    
    start = 0
    ret = []
    
    while match = pattern.match_from(self, start)
      break if limited && limit - ret.size <= 1
      collapsed = match.collapsing?
      if !collapsed || (match.begin(0) != 0)
        ret << match.pre_match_from(last_match ? last_match.end(0) : 0)
        ret.push(*match.captures)        
      end
      if collapsed
        start += 1
      elsif last_match && last_match.collapsing?
        start = match.end(0) + 1
      else
        start = match.end(0)
      end
      last_match = match
    end
    if last_match
      ret << last_match.post_match
    elsif ret.empty?
      ret << self.dup
    end
    (ret.pop while ret[-1] == "") if limit == 0 || limit.nil?
    (ret.shift while ret[0] == "") if spaces
    ret = ret.map {|str| self.class.new(str) } if !self.instance_of?(String)
    ret = ret.map {|str| str.taint} if self.tainted?
    ret
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
  
  # TODO: inspect is NOT dump!
  alias_method :dump, :inspect
  
  def to_sexp(name="(eval)",line=1,newlines=true)
    out = to_sexp_full(name, line, newlines)
    if out.kind_of? Tuple
      exc = SyntaxError.new out.at(0)
      exc.import_position out.at(1), out.at(2)
      raise exc
    end
    return out
  end

  def shared!
    @shared = true
  end
  
  # Replaces the contents and taintedness of <i>string</i> with the corresponding
  # values in <i>other</i>.
  # 
  #   s = "hello"         #=> "hello"
  #   s.replace "world"   #=> "world"
  def replace(other)
    # If we're replacing with ourselves, then we have nothing to do
    return self if self.equal?(other)

    other = StringValue(other)

    raise TypeError, "can't modify frozen string" if self.frozen?

    @shared = true
    other.shared!
    @data = other.data
    @bytes = other.bytes
    @characters = other.characters
    @encoding = other.encoding
    
    self.taint if other.tainted?
    
    self
  end
  alias_method :initialize_copy, :replace
  
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
  
  def swapcase
    (str = self.dup).swapcase! || str
  end
  
  def swapcase!
    return if @bytes == 0
    raise TypeError, "can't modify frozen string" if self.frozen?
  
    if @shared
      @data = @data.dup
      @shared = nil
    end
  
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
    
    if @shared
      @data = @data.dup
      @shared = nil
    end
  
    @bytes.times do |i|
      if @data[i].islower
        @data[i] = @data[i].toupper
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
    
    if @shared
      @data = @data.dup
      @shared = nil
    end
    
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
    while start < @bytes
      c = @data[start]
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

  def gsub_pattern(pattern)
    unless pattern.is_a?(String) || pattern.is_a?(Regexp)
      if pattern.respond_to?(:to_str)
        pattern = pattern.to_str
      else
        raise TypeError, "pattern cannot be converted to a string"
      end
    end
    pattern = Regexp.quote(pattern) if pattern.is_a?(String)
    pattern = Regexp.new(pattern) unless pattern.is_a?(Regexp)
    pattern
  end

  # mini-gsub created purely to handle slashes because using gsub recursively was causing segfaults
  # TODO: make it work correctly
  def replace_slashes
    start = 0
    ret = ""
    while(match = /\\\\/.match_from(self, start))
      ret << match.pre_match_from(start)
      ret << '\\'
      start = match.end(0)
    end
    ret << self[start..-1]
    ret
  end

  def gsub(pattern, replacement = nil, &block)
    raise ArgumentError, "wrong number of arguments (1 for 2)" if !replacement && !block_given?
    raise ArgumentError, "wrong number of arguments (0 for 2)" if pattern.nil?

    raise TypeError, "replacement must be a string" unless replacement.respond_to?(:to_str) || block
    replacement = replacement.to_str if replacement.respond_to?(:to_str)

    taint = self.tainted? || replacement.tainted?

    replacement = replacement.replace_slashes if replacement.is_a?(String)

    start = 0
    last_end = 0
    ret = ""

    $~ = nil

    pattern = gsub_pattern(pattern)
    
    copy = self.dup
    
    
    while(match = pattern.match_from(copy, start))
      $~ = match      
      ret << (match.pre_match_from(last_end) || "")
      if replacement
        # x[-1,1] returns a character version of the last character
        old_md = $~
        ret << replacement.gsub(/\\[\d\&\`\'\+]/) do |x| 
          cap = x[-1,1]
          if cap == '`'
            match.pre_match
          elsif cap == '\''
            match.post_match
          elsif cap == '+'
            match.captures.compact[-1].to_s
          else
            match[cap.to_i]
          end
        end
        $~ = old_md
      else
        old_md = $~
        item = block.call(match[0].dup)
        ret << item.to_s
        $~ = old_md
      end
      last_end = match.end(0)
      start = match.collapsing? ? start + 1 : match.end(0)
    end
    if self != copy
      raise RuntimeError, "You cannot modify the original string"
    end
    ret << self[start..-1] if self[start..-1]
    ret.taint if taint
    ret = self.class.new(ret) unless self.instance_of?(String)
    ret
  end
  
  def gsub!(pattern, replacement = nil, &block)
    pattern = gsub_pattern(pattern)
    if self.frozen? && self =~ pattern
      raise TypeError, "You cannot modify a frozen string" if !block
      raise RuntimeError, "You cannot modify a frozen string" if block
    end
    to_replace = self.gsub(pattern, replacement, &block)
    if self != to_replace
      self.replace(to_replace)
    else
      return nil
    end
  end

  # def gsub(pattern, replacement = nil, &block)
  #   (str = self.dup).gsub!(pattern, replacement, &block) || str
  # end
  # 
  # def gsub!(pattern, replacement = nil)
  #   raise ArgumentError, "wrong number of arguments (1 for 2)" if !replacement && !block_given?
  #   
  #   replacement = StringValue(replacement) if replacement
  #   
  #   pattern = Regexp.quote(pattern) if pattern.is_a?(String)
  #   pattern = Regexp.new(pattern) unless pattern.is_a?(Regexp)
  # 
  #   out = [self]
  #   while (match = pattern.match(out.last)) && out.pop
  #     out << match.pre_match
  #     if block_given?
  #       out << yield(match[0])
  #     else
  #       replacement = replacement.to_str
  #       out << replacement.gsub(/\\\d/) { |x| match[x[-1,1].to_i] }
  #     end
  #     out << match.post_match
  #   end
  # 
  #   out = out.join
  #   out.taint if self.tainted? || (replacement && replacement.tainted?)
  #   out.empty? || out == self ? nil : replace(out)
  # end

  def sub(pattern, replacement = nil, &block)
    (str = self.dup).sub!(pattern, replacement, &block) || str
  end

  def sub!(pattern, replacement = nil)
    raise ArgumentError, "wrong number of arguments (1 for 2)" if !replacement && !block_given?
    
    replacement = StringValue(replacement) if replacement
    
    pattern = Regexp.quote(pattern) if pattern.is_a?(String)
    pattern = Regexp.new(pattern) unless pattern.is_a?(Regexp)
    
    return unless match = pattern.match(self)

    out = [ match.pre_match ]
    if block_given?
      out << yield(match[0])
    else
      replacement = replacement.to_str
      out << replacement.gsub(/\\\d/) { |x| match[x[-1,1].to_i] }
    end
    out << match.post_match
    
    ret = out.join
    ret.taint if self.tainted? || replacement.tainted?
    
    replace(ret)
  end

  def insert(index, other_string)
    other_string = StringValue(other_string)
    
    if index == -1
      return self << other_string
    elsif index < 0
      index += 1
    end
    
    self[index, 0] = other_string
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

  def expand_tr_string(string)
    string.gsub(/[^-]-[^-]/) { |r| (r[0]..r[2]).to_a.map { |c| c.chr } }
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
  def intersect_string_from_args(*args)
    args.map! do |chars|
      chars = StringValue(chars)
      expand_tr_string(chars)
    end

    intersection = args.shift
    args.each do |chars|
      intersection = intersection.split(//).map { |c|
        c if chars[0] == ?^ ? !chars.include?(c) : chars.include?(c)
      }.join
      return if intersection.empty?
    end
    intersection
  end

  def squeeze(*arg)

    out = ""
    last_char = -1

    if arg.length > 0
      # Build the target character map
      str = intersect_string_from_args(*arg)
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
    from_str  = expand_tr_string(from_str)
    to_str    = expand_tr_string(to_str)

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

  # Returns <code>true</code> if <i>self</i> contains the given string or
  # character.
  #    
  #   "hello".include? "lo"   #=> true
  #   "hello".include? "ol"   #=> false
  #   "hello".include? ?h     #=> true
  def include?(needle)
    if needle.is_a? Fixnum
      each_byte { |b| return true if b == needle }
      return false
    end

    !self.index(StringValue(needle)).nil?
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
        break if m.begin(0) == m.end(0)
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

  def slice!(*args)
    result = slice(*args)
    self[*args] = '' unless result.nil?
    result
  end

  def oct
    self.to_i(8)
  end
  
  def hex
    self.to_i(16)
  end
  
  def match(pattern)
    unless pattern.is_a?(String) || pattern.is_a?(Regexp)
      raise TypeError, "wrong argument type #{pattern.class} (expected Regexp)"
    end
    pattern = Regexp.new(pattern) unless Regexp === pattern
    pattern.match(self)
  end
  
  def sum(bits=16)
    sum = 0
    each_byte { |b| sum += b }
    sum & ((1 << bits) - 1)
  end
  
  def upto(stop)
    stop = StringValue(stop)
    raise LocalJumpError, "no block given" unless block_given?

    return self if self > stop

    str = self.dup
    loop do
      yield str.dup
      str.succ!
      break if str.size > stop.size || str > stop
    end
    self
  end

  def each(separator=$/)
    if separator.nil?
      yield self
      return self
    end

    separator = StringValue(separator)
    
    raise LocalJumpError, "no block given" unless block_given?
    
    newline = separator.empty? ? ?\n : separator[separator.length - 1]
    
    last_index = 0
    index = separator.length
    
    old_str = self.dup
    
    while index < self.length
      if separator.empty?
        index += 1 while (self.size > index + 2) && self[(index + 1)..(index + 2)] =~ /[^\n]/
        index += 1 while (self.size > index + 1) && self[index + 1] == ?\n
      end
    
      if self[index] == newline && self[-separator.length, separator.length]
        line = self[last_index..index]
        line.taint if self.tainted?
        yield line
        raise RuntimeError, "You modified the string while running each" if old_str != self
        last_index = index + 1
      end
      
      index += 1
    end
    
    unless last_index == self.length
      line = self[last_index..self.length]
      line.taint if self.tainted?
      yield line
    end

    self
  end
  alias_method :each_line, :each

  def scan(pattern, &block)
    unless pattern.is_a?(String) || pattern.is_a?(Regexp)
      raise TypeError, "wrong argument type #{pattern.class} (expected Regexp)"
    end
    
    pattern = Regexp.new(pattern) unless pattern.is_a?(Regexp)
    index = 0

    unless block_given?
      ret = []
      while index <= self.length and match = pattern.match(self[index..-1])
        if match.begin(0) == match.end(0)
          index += 1
        else
          index += match.end(0)
        end
        
        ret << (match.length > 1 ? match.captures : match[0])
      end
      return ret
    else
      while index <= self.length and match = pattern.match(self[index..-1])
        if match.begin(0) == match.end(0)
          index += 1
        else
          index += match.end(0)
        end
        
        if match.size == 1
          block.call(match[0])
        else
          block.call(*match.captures)
        end
      end
    end
  end
    
  alias_method :eql?, :==

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

# Default Ruby Record Separator
# Used in this file and by various methods that need to ignore $/
DEFAULT_RECORD_SEPARATOR = "\n"

class String
  include Comparable
  include Enumerable

  attr_accessor :data
  attr_accessor :num_bytes
  attr_accessor :characters

  alias_method :__data__, :data
  alias_method :__data__=, :data=

  def self.allocate
    str = super()
    str.__data__ = Rubinius::ByteArray.new(1)
    str.num_bytes = 0
    str.characters = 0
    str
  end

  ##
  # Creates a new string from copying _count_ bytes from the
  # _start_ of _ba_.
  def self.from_bytearray(ba, start, count)
    Ruby.primitive :string_from_bytearray
    raise PrimitiveFailure, "String.from_bytearray primitive failed"
  end

  class << self
    def clone
      raise TypeError, "Unable to clone/dup String class"
    end

    alias_method :dup, :clone
  end

  def initialize(arg = undefined)
    replace StringValue(arg) unless arg.equal?(undefined)

    self
  end

  private :initialize

  # call-seq:
  #   str % arg   => new_str
  #
  # Format---Uses <i>self</i> as a format specification, and returns the result
  # of applying it to <i>arg</i>. If the format specification contains more than
  # one substitution, then <i>arg</i> must be an <code>Array</code> containing
  # the values to be substituted. See <code>Kernel::sprintf</code> for details
  # of the format string.
  #
  #   "%05d" % 123                       #=> "00123"
  #   "%-5s: %08x" % [ "ID", self.id ]   #=> "ID   : 200e14d6"
  def %(args)
    if args.respond_to?(:to_ary)
      ret = Rubinius::Sprinter.get(self).call(*args)
    else
      ret = Rubinius::Sprinter.get(self).call(args)
    end

    ret.taint if tainted?
    return ret
  end

  # call-seq:
  #   str * integer   => new_str
  #
  # Copy --- Returns a new <code>String</code> containing <i>integer</i> copies of
  # the receiver.
  #
  #   "Ho! " * 3   #=> "Ho! Ho! Ho! "
  def *(num)
    num = Rubinius::Type.coerce_to(num, Integer, :to_int) unless num.kind_of? Integer

    if num.kind_of? Bignum
      raise RangeError, "bignum too big to convert into `long' (#{num})"
    end

    if num < 0
      raise ArgumentError, "unable to multiple negative times (#{num})"
    end

    str = self.class.pattern num * @num_bytes, self
    return str
  end

  # Concatenation --- Returns a new <code>String</code> containing
  # <i>other</i> concatenated to <i>string</i>.
  #
  #   "Hello from " + self.to_s   #=> "Hello from main"
  def +(other)
    r = "#{self}#{StringValue(other)}"
    r.taint if tainted? or other.tainted?
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
    modify!

    unless other.kind_of? String
      if other.kind_of?(Integer) && other >= 0 && other <= 255
        other = other.chr
      else
        other = StringValue(other)
      end
    end

    taint if other.tainted?
    append(other)
  end
  alias_method :concat, :<<

  # call-seq:
  #   str <=> other_str   => -1, 0, +1
  #
  # Comparison --- Returns -1 if <i>other_str</i> is less than, 0 if
  # <i>other_str</i> is equal to, and +1 if <i>other_str</i> is greater than
  # <i>self</i>. If the strings are of different lengths, and the strings are
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
    if other.kind_of?(String)
      @data.compare_bytes(other.__data__, @num_bytes, other.size)
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
  # <code>false</code>. Otherwise, returns <code>true</code> if <i>self</i>
  # <code><=></code> <i>obj</i> returns zero.
  def ==(other)
    Ruby.primitive :string_equal

    # Use #=== rather than #kind_of? because other might redefine kind_of?
    unless String === other
      if other.respond_to?(:to_str)
        return other == self
      end
      return false
    end

    return false unless @num_bytes == other.size
    return @data.compare_bytes(other.__data__, @num_bytes, other.size) == 0
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
      match_data = pattern.search_region(self, 0, @num_bytes, true)
      Regexp.last_match = match_data
      return match_data.full[0] if match_data
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
  # an offset is negative, it is counted from the end of <i>self</i>. Returns
  # <code>nil</code> if the initial offset falls outside the string, the length
  # is negative, or the beginning of the range is greater than the end.
  #
  # If a <code>Regexp</code> is supplied, the matching portion of <i>self</i> is
  # returned. If a numeric parameter follows the regular expression, that
  # component of the <code>MatchData</code> is returned instead. If a
  # <code>String</code> is given, that string is returned if it occurs in
  # <i>self</i>. In both cases, <code>nil</code> is returned if there is no
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
  def [](index, other = undefined)
    unless other.equal?(undefined)
      length = Rubinius::Type.coerce_to(other, Fixnum, :to_int)

      if index.kind_of? Regexp
        match, str = subpattern(index, length)
        Regexp.last_match = match
        return str
      else
        start  = Rubinius::Type.coerce_to(index, Fixnum, :to_int)
        return substring(start, length)
      end
    end

    case index
    when Regexp
      match_data = index.search_region(self, 0, @num_bytes, true)
      Regexp.last_match = match_data
      if match_data
        result = match_data.to_s
        result.taint if index.tainted?
        return result
      end
    when String
      return include?(index) ? index.dup : nil
    when Range
      start   = Rubinius::Type.coerce_to index.first, Fixnum, :to_int
      length  = Rubinius::Type.coerce_to index.last,  Fixnum, :to_int

      start += @num_bytes if start < 0

      length += @num_bytes if length < 0
      length += 1 unless index.exclude_end?

      return "" if start == @num_bytes
      return nil if start < 0 || start > @num_bytes

      length = @num_bytes if length > @num_bytes
      length = length - start
      length = 0 if length < 0

      return substring(start, length)
    # A really stupid case hit for rails. Either we define this or we define
    # Symbol#to_int. We removed Symbol#to_int in late 2007 because it's evil,
    # and do not want to re add it.
    when Symbol
      return nil
    else
      index = Rubinius::Type.coerce_to index, Fixnum, :to_int
      index = @num_bytes + index if index < 0

      return if index < 0 || @num_bytes <= index
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
  # Element Assignment --- Replaces some or all of the content of <i>self</i>. The
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
  def []=(index, replacement, three=undefined)
    unless three.equal?(undefined)
      if index.kind_of? Regexp
        subpattern_set index,
                       Rubinius::Type.coerce_to(replacement, Integer, :to_int),
                       three
      else
        start = Rubinius::Type.coerce_to(index, Integer, :to_int)
        fin =   Rubinius::Type.coerce_to(replacement, Integer, :to_int)

        splice! start, fin, three
      end

      return three
    end

    case index
    when Fixnum
      # Handle this first because it's the most common.
      # This is duplicated from the else branch, but don't dry it up.
      if index < 0
        index += @num_bytes
        if index < 0 or index >= @num_bytes
          raise IndexError, "index #{index} out of string"
        end
      else
        raise IndexError, "index #{index} out of string" if index >= @num_bytes
      end

      if replacement.kind_of?(Fixnum)
        modify!
        @data[index] = replacement
      else
        splice! index, 1, replacement
      end
    when Regexp
      subpattern_set index, 0, replacement
    when String
      unless start = self.index(index)
        raise IndexError, "string not matched"
      end

      splice! start, index.length, replacement
    when Range
      start   = Rubinius::Type.coerce_to(index.first, Integer, :to_int)
      length  = Rubinius::Type.coerce_to(index.last, Integer, :to_int)

      start += @num_bytes if start < 0

      return nil if start < 0 || start > @num_bytes

      length = @num_bytes if length > @num_bytes
      length += @num_bytes if length < 0
      length += 1 unless index.exclude_end?

      length = length - start
      length = 0 if length < 0

      splice! start, length, replacement
    else
      index = Rubinius::Type.coerce_to(index, Integer, :to_int)
      raise IndexError, "index #{index} out of string" if @num_bytes <= index

      if index < 0
        raise IndexError, "index #{index} out of string" if -index > @num_bytes
        index += @num_bytes
      end

      if replacement.kind_of?(Fixnum)
        modify!
        @data[index] = replacement
      else
        splice! index, 1, replacement
      end
    end
    return replacement
  end

  alias_method :bytesize, :length

  # Returns a copy of <i>self</i> with the first character converted to uppercase
  # and the remainder to lowercase.
  # Note: case conversion is effective only in ASCII region.
  #
  #   "hello".capitalize    #=> "Hello"
  #   "HELLO".capitalize    #=> "Hello"
  #   "123ABC".capitalize   #=> "123abc"
  def capitalize
    return dup if @num_bytes == 0

    str = transform(Rubinius::CType::Lowered, true)
    str = self.class.new(str) unless instance_of?(String)

    str.modify!

    # Now do the actual capitalization
    ba = str.__data__
    ba[0] = Rubinius::CType.toupper(ba[0])

    return str
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
    Ruby.check_frozen

    cap = capitalize()
    return nil if cap == self

    replace(cap)
    return self
  end

  # Case-insensitive version of <code>String#<=></code>.
  #
  #   "abcdef".casecmp("abcde")     #=> 1
  #   "aBcDeF".casecmp("abcdef")    #=> 0
  #   "abcdef".casecmp("abcdefg")   #=> -1
  #   "abcdef".casecmp("ABCDEF")    #=> 0
  def casecmp(to)
    to = StringValue(to)
    order = @num_bytes - to.num_bytes
    size = order < 0 ? @num_bytes : to.num_bytes

    ctype = Rubinius::CType

    i = 0
    while i < size
      a = @data[i]
      b = to.__data__[i]
      i += 1

      a = ctype.toupper!(a) if ctype.islower(a)
      b = ctype.toupper!(b) if ctype.islower(b)
      r = a - b

      next if r == 0
      return r < 0 ? -1 : 1
    end

    return 0 if order == 0
    return order < 0 ? -1 : 1
  end

  # If <i>integer</i> is greater than the length of <i>self</i>, returns a new
  # <code>String</code> of length <i>integer</i> with <i>self</i> centered and
  # padded with <i>padstr</i>; otherwise, returns <i>self</i>.
  #
  #    "hello".center(4)         #=> "hello"
  #    "hello".center(20)        #=> "       hello        "
  #    "hello".center(20, '123') #=> "1231231hello12312312"
  def center(width, padstr = " ")
    justify width, :center, padstr
  end

  # Returns a new <code>String</code> with the given record separator removed
  # from the end of <i>self</i> (if present). If <code>$/</code> has not been
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
  def chomp(separator=$/)
    str = dup
    str.chomp!(separator) || str
  end

  # Modifies <i>self</i> in place as described for <code>String#chomp</code>,
  # returning <i>self</i>, or <code>nil</code> if no modifications were made.
  #---
  # NOTE: TypeError is raised in String#replace and not in String#chomp! when
  # self is frozen. This is intended behaviour.
  #+++
  def chomp!(sep=undefined)
    # special case for performance. No seperator is by far the most common usage.
    if sep.equal?(undefined)
      return if @num_bytes == 0

      Ruby.check_frozen

      c = @data[@num_bytes-1]
      if c == ?\n
        @num_bytes -= 1 if @num_bytes > 1 && @data[@num_bytes-2] == ?\r
      elsif c != ?\r
        return
      end

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = @characters = @num_bytes - 1
      return self
    end

    return if sep.nil? || @num_bytes == 0
    sep = StringValue sep

    if (sep == $/ && sep == DEFAULT_RECORD_SEPARATOR) || sep == "\n"
      c = @data[@num_bytes-1]
      if c == ?\n
        @num_bytes -= 1 if @num_bytes > 1 && @data[@num_bytes-2] == ?\r
      elsif c != ?\r
        return
      end

      Ruby.check_frozen

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = @characters = @num_bytes - 1
    elsif sep.size == 0
      size = @num_bytes
      while size > 0 && @data[size-1] == ?\n
        if size > 1 && @data[size-2] == ?\r
          size -= 2
        else
          size -= 1
        end
      end

      return if size == @num_bytes

      Ruby.check_frozen

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = @characters = size
    else
      size = sep.size
      return if size > @num_bytes || sep.compare_substring(self, -size, size) != 0

      Ruby.check_frozen

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = @characters = @num_bytes - size
    end

    return self
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
    str = dup
    str.chop! || str
  end

  # Processes <i>self</i> as for <code>String#chop</code>, returning <i>self</i>,
  # or <code>nil</code> if <i>self</i> is the empty string.  See also
  # <code>String#chomp!</code>.
  def chop!
    return if @num_bytes == 0

    self.modify!

    if @num_bytes > 1 and @data[@num_bytes-1] == ?\n \
                      and @data[@num_bytes-2] == ?\r
      @num_bytes = @characters = @num_bytes - 2
    else
      @num_bytes = @characters = @num_bytes - 1
    end

    self
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

    return 0 if @num_bytes == 0

    table = count_table(*strings).__data__

    count = i = 0
    while i < @num_bytes
      count += 1 if table[@data[i]] == 1
      i += 1
    end

    count
  end

  # Applies a one-way cryptographic hash to <i>self</i> by invoking the standard
  # library function <code>crypt</code>. The argument is the salt string, which
  # should be two characters long, each character drawn from
  # <code>[a-zA-Z0-9./]</code>.
  def crypt(other_str)
    other_str = StringValue(other_str)

    if other_str.size < 2
      raise ArgumentError, "salt must be at least 2 characters"
    end

    hash = __crypt__(other_str)
    hash.taint if tainted? || other_str.tainted?
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
  def delete(*strings)
    str = dup
    str.delete!(*strings) || str
  end

  # Performs a <code>delete</code> operation in place, returning <i>self</i>, or
  # <code>nil</code> if <i>self</i> was not modified.
  def delete!(*strings)
    raise ArgumentError, "wrong number of arguments" if strings.empty?
    self.modify!

    table = count_table(*strings).__data__

    i, j = 0, -1
    while i < @num_bytes
      c = @data[i]
      unless table[c] == 1
        @data[j+=1] = c
      end
      i += 1
    end

    if (j += 1) < @num_bytes
      @num_bytes = j
      self
    else
      nil
    end
  end

  # Returns a copy of <i>self</i> with all uppercase letters replaced with their
  # lowercase counterparts. The operation is locale insensitive---only
  # characters ``A'' to ``Z'' are affected.
  #
  # "hEllO".downcase   #=> "hello"
  def downcase
    return dup if @num_bytes == 0
    str = transform(Rubinius::CType::Lowered, true)
    str = self.class.new(str) unless instance_of?(String)

    return str
  end

  # Downcases the contents of <i>self</i>, returning <code>nil</code> if no
  # changes were made.
  def downcase!
    Ruby.check_frozen

    return if @num_bytes == 0

    str = transform(Rubinius::CType::Lowered, true)

    return nil if str == self

    replace(str)

    return self
  end

  def each_char(&block)
    return to_enum :each_char unless block_given?
    return scan(/./u, &block) if Rubinius.kcode == :UTF8
    i = 0
    while i < @num_bytes do
      yield @data.get_byte(i).chr
      i += 1
    end

    self
  end

  alias_method :chars, :each_char

  # Passes each byte in <i>self</i> to the given block.
  #
  #   "hello".each_byte {|c| print c, ' ' }
  #
  # <em>produces:</em>
  #
  #   104 101 108 108 111
  def each_byte
    return to_enum :each_byte unless block_given?
    i = 0
    while i < @num_bytes do
      yield @data.get_byte(i)
      i += 1
    end
    self
  end

  alias_method :bytes, :each_byte

  # Splits <i>self</i> using the supplied parameter as the record separator
  # (<code>$/</code> by default), passing each substring in turn to the supplied
  # block. If a zero-length record separator is supplied, the string is split on
  # <code>\n</code> characters, except that multiple successive newlines are
  # appended together.
  #
  #   print "Example one\n"
  #   "hello\nworld".each {|s| p s}
  #   print "Example two\n"
  #   "hello\nworld".each('l') {|s| p s}
  #   print "Example three\n"
  #   "hello\n\n\nworld".each('') {|s| p s}
  #
  # <em>produces:</em>
  #
  #   Example one
  #   "hello\n"
  #   "world"
  #   Example two
  #   "hel"
  #   "l"
  #   "o\nworl"
  #   "d"
  #   Example three
  #   "hello\n\n\n"
  #   "world"
  def each_line(sep=$/)
    return to_enum(:each_line, sep) unless block_given?

    # weird edge case.
    if sep.nil?
      yield self
      return self
    end

    sep = StringValue(sep)

    pos = 0

    size = @num_bytes
    orig_data = @data

    # If the separator is empty, we're actually in paragraph mode. This
    # is used so infrequently, we'll handle it completely separately from
    # normal line breaking.
    if sep.empty?
      sep = "\n\n"
      pat_size = 2

      while pos < size
        nxt = find_string(sep, pos)
        break unless nxt

        while @data[nxt] == ?\n and nxt < @num_bytes
          nxt += 1
        end

        match_size = nxt - pos

        # string ends with \n's
        break if pos == @num_bytes

        str = substring(pos, match_size)
        yield str unless str.empty?

        # detect mutation within the block
        if !@data.equal?(orig_data) or @num_bytes != size
          raise RuntimeError, "string modified while iterating"
        end

        pos = nxt
      end

      # No more separates, but we need to grab the last part still.
      fin = substring(pos, @num_bytes - pos)
      yield fin if fin and !fin.empty?

    else

      # This is the normal case.
      pat_size = sep.size

      while pos < size
        nxt = find_string(sep, pos)
        break unless nxt

        match_size = nxt - pos
        str = substring(pos, match_size + pat_size)
        yield str unless str.empty?

        # detect mutation within the block
        if !@data.equal?(orig_data) or @num_bytes != size
          raise RuntimeError, "string modified while iterating"
        end

        pos = nxt + pat_size
      end

      # No more separates, but we need to grab the last part still.
      fin = substring(pos, @num_bytes - pos)
      yield fin unless fin.empty?
    end

    self
  end

  alias_method :each, :each_line
  alias_method :lines, :each_line


  # Returns <code>true</code> if <i>self</i> has a length of zero.
  #
  #   "hello".empty?   #=> false
  #   "".empty?        #=> true
  def empty?
    @num_bytes == 0
  end

  def end_with?(*suffixes)
    suffixes.each do |suffix|
      suffix = Rubinius::Type.try_convert suffix, String, :to_str
      next unless suffix

      return true if self[-suffix.length, suffix.length] == suffix
    end
    false
  end

  # Two strings are equal if the have the same length and content.
  def eql?(other)
    Ruby.primitive :string_equal

    return false unless other.kind_of?(String) && other.size == @num_bytes
    return @data.compare_bytes(other.__data__, @num_bytes, other.size) == 0
  end

  # Returns a copy of <i>self</i> with <em>all</em> occurrences of <i>pattern</i>
  # replaced with either <i>replacement</i> or the value of the block. The
  # <i>pattern</i> will typically be a <code>Regexp</code>; if it is a
  # <code>String</code> then no regular expression metacharacters will be
  # interpreted (that is <code>/\d/</code> will match a digit, but
  # <code>'\d'</code> will match a backslash followed by a 'd').
  #
  # If a string is used as the replacement, special variables from the match
  # (such as <code>$&</code> and <code>$1</code>) cannot be substituted into it,
  # as substitution into the string occurs before the pattern match
  # starts. However, the sequences <code>\1</code>, <code>\2</code>, and so on
  # may be used to interpolate successive groups in the match.
  #
  # In the block form, the current match string is passed in as a parameter, and
  # variables such as <code>$1</code>, <code>$2</code>, <code>$`</code>,
  # <code>$&</code>, and <code>$'</code> will be set appropriately. The value
  # returned by the block will be substituted for the match on each call.
  #
  # The result inherits any tainting in the original string or any supplied
  # replacement string.
  #
  #   "hello".gsub(/[aeiou]/, '*')              #=> "h*ll*"
  #   "hello".gsub(/([aeiou])/, '<\1>')         #=> "h<e>ll<o>"
  #   "hello".gsub(/./) {|s| s[0].to_s + ' '}   #=> "104 101 108 108 111 "
  def gsub(pattern, replacement=undefined)
    unless block_given? or replacement != undefined
      return to_enum(:gsub, pattern, replacement)
    end

    tainted = false

    if replacement.equal?(undefined)
      use_yield = true
    else
      tainted = replacement.tainted?
      replacement = StringValue(replacement)
      tainted ||= replacement.tainted?
      use_yield = false
    end

    pattern = get_pattern(pattern, true)
    orig_len = @num_bytes
    orig_data = @data

    last_end = 0
    offset = nil
    ret = substring(0,0) # Empty string and string subclass

    last_match = nil
    match = pattern.match_from self, last_end

    if match
      ma_range = match.full
      ma_start = ma_range.at(0)
      ma_end   = ma_range.at(1)

      offset = ma_start
    end

    while match
      nd = ma_start - 1
      pre_len = nd-last_end+1

      if pre_len > 0
        ret.append substring(last_end, pre_len)
      end

      if use_yield
        Regexp.last_match = match

        val = yield match.to_s

        val = val.to_s unless val.kind_of?(String)

        tainted ||= val.tainted?
        ret.append val

        if !@data.equal?(orig_data) or @num_bytes != orig_len
          raise RuntimeError, "string modified"
        end
      else
        ret.append replacement.to_sub_replacement(match)
      end

      tainted ||= val.tainted?

      last_end = ma_end

      if ma_start == ma_end
        if char = find_character(offset)
          offset += char.size
        else
          offset += 1
        end
      else
        offset = ma_end
      end

      last_match = match

      match = pattern.match_from self, offset
      break unless match

      ma_range = match.full
      ma_start = ma_range.at(0)
      ma_end   = ma_range.at(1)

      offset = ma_start
    end

    Regexp.last_match = last_match

    str = substring(last_end, @num_bytes-last_end+1)
    ret.append str if str

    ret.taint if tainted || self.tainted?
    return ret
  end

  # Performs the substitutions of <code>String#gsub</code> in place, returning
  # <i>self</i>, or <code>nil</code> if no substitutions were performed.
  def gsub!(pattern, replacement=undefined)
    # Because of the behavior of $~, this is duplicated from gsub! because
    # if we call gsub! from gsub, the last_match can't be updated properly.
    unless block_given? or replacement != undefined
      return to_enum(:gsub, pattern, replacement)
    end

    tainted = false

    unless replacement.equal?(undefined)
      tainted = replacement.tainted?
      replacement = StringValue(replacement)
      tainted ||= replacement.tainted?
    end

    pattern = get_pattern(pattern, true)
    orig_len = @num_bytes

    last_end = 0
    offset = nil
    ret = substring(0,0) # Empty string and string subclass

    last_match = nil
    match = pattern.match_from self, last_end

    offset = match.begin 0 if match

    while match
      if str = match.pre_match_from(last_end)
        ret.append str
      end

      if replacement.equal?(undefined)
        Regexp.last_match = match

        val = yield(match[0]).to_s
        tainted ||= val.tainted?
        ret.append val

        raise RuntimeError, "string modified" unless @num_bytes == orig_len
      else
        ret.append replacement.to_sub_replacement(match)
      end

      tainted ||= val.tainted?

      last_end = match.end(0)

      if match.collapsing?
        if char = find_character(offset)
          offset += char.size
        else
          offset += 1
        end
      else
        offset = match.end(0)
      end

      last_match = match

      match = pattern.match_from self, offset
      break unless match

      offset = match.begin 0
    end

    Regexp.last_match = last_match

    str = substring(last_end, @num_bytes-last_end+1)
    ret.append str if str

    ret.taint if tainted || self.tainted?

    if last_match
      replace(ret)
      return self
    else
      return nil
    end
  end

  # Treats leading characters from <i>self</i> as a string of hexadecimal digits
  # (with an optional sign and an optional <code>0x</code>) and returns the
  # corresponding number. Zero is returned on error.
  #
  #    "0x0a".hex     #=> 10
  #    "-1234".hex    #=> -4660
  #    "0".hex        #=> 0
  #    "wombat".hex   #=> 0
  def hex
    to_inum(16, false)
  end

  # Returns <code>true</code> if <i>self</i> contains the given string or
  # character.
  #
  #   "hello".include? "lo"   #=> true
  #   "hello".include? "ol"   #=> false
  #   "hello".include? ?h     #=> true
  def include?(needle)
    if needle.kind_of? Fixnum
      needle = needle % 256
      str_needle = needle.chr
    else
      str_needle = StringValue(needle)
    end

    !!find_string(str_needle, 0)
  end

  # Returns the index of the first occurrence of the given <i>substring</i>,
  # character (<i>fixnum</i>), or pattern (<i>regexp</i>) in <i>self</i>. Returns
  # <code>nil</code> if not found. If the second parameter is present, it
  # specifies the position in the string to begin the search.
  #
  #   "hello".index('e')             #=> 1
  #   "hello".index('lo')            #=> 3
  #   "hello".index('a')             #=> nil
  #   "hello".index(101)             #=> 1
  #   "hello".index(/[aeiou]/, -3)   #=> 4
  def index(needle, offset=0)
    offset = Rubinius::Type.coerce_to offset, Integer, :to_int
    offset = @num_bytes + offset if offset < 0

    return nil if offset < 0 || offset > @num_bytes

    needle = needle.to_str if !needle.instance_of?(String) && needle.respond_to?(:to_str)

    # What are we searching for?
    case needle
    when Fixnum
      return nil if needle > 255 or needle < 0
      return find_string(needle.chr, offset)
    when String
      return offset if needle == ""

      needle_size = needle.size

      max = @num_bytes - needle_size
      return if max < 0 # <= 0 maybe?

      return find_string(needle, offset)
    when Regexp
      if match = needle.match_from(self, offset)
        Regexp.last_match = match
        return match.begin(0)
      else
        Regexp.last_match = nil
      end
    else
      raise TypeError, "type mismatch: #{needle.class} given"
    end

    return nil
  end

  # Inserts <i>other_string</i> before the character at the given
  # <i>index</i>, modifying <i>self</i>. Negative indices count from the
  # end of the string, and insert <em>after</em> the given character.
  # The intent is insert <i>other_string</i> so that it starts at the given
  # <i>index</i>.
  #
  #   "abcd".insert(0, 'X')    #=> "Xabcd"
  #   "abcd".insert(3, 'X')    #=> "abcXd"
  #   "abcd".insert(4, 'X')    #=> "abcdX"
  #   "abcd".insert(-3, 'X')   #=> "abXcd"
  #   "abcd".insert(-1, 'X')   #=> "abcdX"
  def insert(index, other)
    other = StringValue(other)
    index = Rubinius::Type.coerce_to index, Fixnum, :to_int

    osize = other.size
    size = @num_bytes + osize
    str = self.class.new("\0") * size

    index = @num_bytes + 1 + index if index < 0
    if index > @num_bytes or index < 0 then
      raise IndexError, "index #{index} out of string"
    end

    modify!

    if index == 0
      str.copy_from other, 0, other.size, 0
      str.copy_from self, 0, @num_bytes, other.size
    elsif index < @num_bytes
      str.copy_from self, 0, index, 0
      str.copy_from other, 0, osize, index
      str.copy_from self, index, @num_bytes - index, index + osize
    else
      str.copy_from self, 0, @num_bytes, 0
      str.copy_from other, 0, other.size, @num_bytes
    end

    @num_bytes = size
    @data = str.__data__
    taint if other.tainted?

    self
  end

  ControlCharacters = [?\n, ?\t, ?\a, ?\v, ?\f, ?\r, ?\e, ?\b]
  ControlPrintValue = ["\\n", "\\t", "\\a", "\\v", "\\f", "\\r", "\\e", "\\b"]

  # Returns a printable version of _self_, with special characters
  # escaped.
  #
  #   str = "hello"
  #   str[3] = 8
  #   str.inspect       #=> "hel\010o"
  def inspect
    "\"#{transform(Rubinius::CType::Printed, true)}\""
  end

  # If <i>integer</i> is greater than the length of <i>self</i>, returns a new
  # <code>String</code> of length <i>integer</i> with <i>self</i> left justified
  # and padded with <i>padstr</i>; otherwise, returns <i>self</i>.
  #
  #   "hello".ljust(4)            #=> "hello"
  #   "hello".ljust(20)           #=> "hello               "
  #   "hello".ljust(20, '1234')   #=> "hello123412341234123"
  def ljust(width, padstr=" ")
    justify(width, :left, padstr)
  end

  # Returns a copy of <i>self</i> with leading whitespace removed. See also
  # <code>String#rstrip</code> and <code>String#strip</code>.
  #
  #   "  hello  ".lstrip   #=> "hello  "
  #   "hello".lstrip       #=> "hello"
  def lstrip
    str = dup
    str.lstrip! || str
  end

  # Removes leading whitespace from <i>self</i>, returning <code>nil</code> if no
  # change was made. See also <code>String#rstrip!</code> and
  # <code>String#strip!</code>.
  #
  #   "  hello  ".lstrip   #=> "hello  "
  #   "hello".lstrip!      #=> nil
  def lstrip!
    return if @num_bytes == 0

    start = 0

    ctype = Rubinius::CType

    while start < @num_bytes && ctype.isspace(@data[start])
      start += 1
    end

    return if start == 0

    modify!
    @num_bytes = @characters = @num_bytes - start
    @data.move_bytes start, @num_bytes, 0
    self
  end

  # Converts <i>pattern</i> to a <code>Regexp</code> (if it isn't already one),
  # then invokes its <code>match</code> method on <i>self</i>.
  #
  #   'hello'.match('(.)\1')      #=> #<MatchData:0x401b3d30>
  #   'hello'.match('(.)\1')[0]   #=> "ll"
  #   'hello'.match(/(.)\1/)[0]   #=> "ll"
  #   'hello'.match('xx')         #=> nil
  def match(pattern)
    match_data = get_pattern(pattern).search_region(self, 0, @num_bytes, true)
    Regexp.last_match = match_data
    return match_data
  end

  # Treats leading characters of <i>self</i> as a string of octal digits (with an
  # optional sign) and returns the corresponding number. Returns 0 if the
  # conversion fails.
  #
  #   "123".oct       # => 83
  #   "-377".oct      # => -255
  #   "bad".oct       # => 0
  #   "0377bad".oct   # => 255
  #
  # Any valid base identifier, if found, is honored though. For instance:
  #
  #   "0b1010".oct    # => 10
  #   "0xff".oct      # => 256
  #
  # If a valid base identifier is not found, the string is assumed to be base 8.
  def oct
    to_inum(-8, false)
  end

  # Replaces the contents and taintedness of <i>string</i> with the corresponding
  # values in <i>other</i>.
  #
  #   s = "hello"         #=> "hello"
  #   s.replace "world"   #=> "world"
  def replace(other)
    # If we're replacing with ourselves, then we have nothing to do
    return self if equal?(other)

    Ruby.check_frozen

    other = StringValue(other)

    @shared = true
    other.shared!
    @data = other.__data__
    @num_bytes = other.num_bytes
    @characters = other.characters
    @hash_value = nil

    taint if other.tainted?

    self
  end
  alias_method :initialize_copy, :replace
  # private :initialize_copy

  # Returns a new string with the characters from <i>self</i> in reverse order.
  #
  #   "stressed".reverse   #=> "desserts"
  def reverse
    dup.reverse!
  end

  # Reverses <i>self</i> in place.
  def reverse!
    return self if @num_bytes <= 1
    self.modify!

    @data.reverse(0, @num_bytes)
    self
  end

  # Returns the index of the last occurrence of the given <i>substring</i>,
  # character (<i>fixnum</i>), or pattern (<i>regexp</i>) in <i>self</i>. Returns
  # <code>nil</code> if not found. If the second parameter is present, it
  # specifies the position in the string to end the search---characters beyond
  # this point will not be considered.
  #
  #   "hello".rindex('e')             #=> 1
  #   "hello".rindex('l')             #=> 3
  #   "hello".rindex('a')             #=> nil
  #   "hello".rindex(101)             #=> 1
  #   "hello".rindex(/[aeiou]/, -2)   #=> 1
  def rindex(sub, finish=undefined)
    if finish.equal?(undefined)
      finish = size
    else
      finish = Rubinius::Type.coerce_to(finish, Integer, :to_int)
      finish += @num_bytes if finish < 0
      return nil if finish < 0
      finish = @num_bytes if finish >= @num_bytes
    end

    case sub
    when Fixnum
      if finish == size
        return nil if finish == 0
        finish -= 1
      end

      begin
        str = sub.chr
      rescue RangeError
        return nil
      end

      return find_string_reverse(str, finish)

    when Regexp
      match_data = sub.search_region(self, 0, finish, false)
      Regexp.last_match = match_data
      return match_data.begin(0) if match_data

    else
      needle = StringValue(sub)
      needle_size = needle.size

      # needle is bigger that haystack
      return nil if size < needle_size

      # Boundary case
      return finish if needle_size == 0

      return find_string_reverse(needle, finish)
    end

    return nil
  end

  # call-seq: str.partition(sep) => [head, sep, tail]
  #
  # Searches the string for _sep_ and returns the part before it, the
  # _sep_, and the part after it. If _sep_ is not found, returns _str_
  # and two empty strings. If no argument is given,
  # Enumerable#partition is called.
  #
  #    "hello".partition("l")         #=> ["he", "l", "lo"]
  #    "hello".partition("x")         #=> ["hello", "", ""]
  #
  def partition(pattern=nil)
    return super() if block_given?

    if pattern.kind_of? Regexp
      if m = pattern.match(self)
        return [m.pre_match, m.to_s, m.post_match]
      end
    else
      pattern = StringValue(pattern)
      if i = index(pattern)
        post_start = i + pattern.length
        post_len = size - post_start

        return [substring(0, i),
                pattern.dup,
                substring(post_start, post_len)]
      end
    end

    # Nothing worked out, this is the default.
    return [self, "", ""]
  end

  # call-seq:
  #
  #    str.rpartition(sep)             => [head, sep, tail]
  #    str.rpartition(regexp)          => [head, match, tail]
  #
  # Searches _sep_ or pattern (_regexp_) in the string from the end
  # of the string, and returns the part before it, the match, and the part
  # after it.
  # If it is not found, returns two empty strings and _str_.
  #
  #    "hello".rpartition("l")         #=> ["hel", "l", "o"]
  #    "hello".rpartition("x")         #=> ["", "", "hello"]
  #    "hello".rpartition(/.l/)        #=> ["he", "ll", "o"]
  #
  def rpartition(pattern)
    if pattern.kind_of? Regexp
      if m = pattern.search_region(self, 0, size, false)
        Regexp.last_match = m
        [m.pre_match, m[0], m.post_match]
      end
    else
      pattern = StringValue(pattern)
      if i = rindex(pattern)
        post_start = i + pattern.length
        post_len = size - post_start

        return [substring(0, i),
                pattern.dup,
                substring(post_start, post_len)]
      end

      # Nothing worked out, this is the default.
      return ["", "", self]
    end
  end

  # If <i>integer</i> is greater than the length of <i>self</i>, returns a new
  # <code>String</code> of length <i>integer</i> with <i>self</i> right justified
  # and padded with <i>padstr</i>; otherwise, returns <i>self</i>.
  #
  #   "hello".rjust(4)            #=> "hello"
  #   "hello".rjust(20)           #=> "               hello"
  #   "hello".rjust(20, '1234')   #=> "123412341234123hello"
  def rjust(width, padstr = " ")
    justify(width, :right, padstr)
  end

  # Returns a copy of <i>self</i> with trailing whitespace removed. See also
  # <code>String#lstrip</code> and <code>String#strip</code>.
  #
  #   "  hello  ".rstrip   #=> "  hello"
  #   "hello".rstrip       #=> "hello"
  def rstrip
    str = dup
    str.rstrip! || str
  end

  # Removes trailing whitespace from <i>self</i>, returning <code>nil</code> if
  # no change was made. See also <code>String#lstrip!</code> and
  # <code>String#strip!</code>.
  #
  #   "  hello  ".rstrip   #=> "  hello"
  #   "hello".rstrip!      #=> nil
  def rstrip!
    return if @num_bytes == 0

    stop = @num_bytes - 1

    while stop >= 0 && @data[stop] == 0
      stop -= 1
    end

    ctype = Rubinius::CType

    while stop >= 0 && ctype.isspace(@data[stop])
      stop -= 1
    end

    return if (stop += 1) == @num_bytes

    modify!
    @num_bytes = @characters = stop
    self
  end


  # Both forms iterate through <i>self</i>, matching the pattern (which may be a
  # <code>Regexp</code> or a <code>String</code>). For each match, a result is
  # generated and either added to the result array or passed to the block. If
  # the pattern contains no groups, each individual result consists of the
  # matched string, <code>$&</code>.  If the pattern contains groups, each
  # individual result is itself an array containing one entry per group.
  #
  #   a = "cruel world"
  #   a.scan(/\w+/)        #=> ["cruel", "world"]
  #   a.scan(/.../)        #=> ["cru", "el ", "wor"]
  #   a.scan(/(...)/)      #=> [["cru"], ["el "], ["wor"]]
  #   a.scan(/(..)(..)/)   #=> [["cr", "ue"], ["l ", "wo"]]
  #
  # And the block form:
  #
  #   a.scan(/\w+/) {|w| print "<<#{w}>> " }
  #   print "\n"
  #   a.scan(/(.)(.)/) {|x,y| print y, x }
  #   print "\n"
  #
  # <em>produces:</em>
  #
  #   <<cruel>> <<world>>
  #   rceu lowlr

  def scan(pattern)
    taint = tainted? || pattern.tainted?
    pattern = get_pattern(pattern, true)
    index = 0

    last_match = nil

    if block_given?
      ret = self
    else
      ret = []
    end

    while match = pattern.match_from(self, index)
      fin = match.end(0)

      if match.collapsing?
        if char = find_character(fin)
          index = fin + char.size
        else
          index = fin + 1
        end
      else
        index = fin
      end

      last_match = match
      val = (match.length == 1 ? match[0] : match.captures)
      val.taint if taint

      if block_given?
        Regexp.last_match = match
        yield(val)
      else
        ret << val
      end
    end

    Regexp.last_match = last_match
    return ret
  end

  # Deletes the specified portion from <i>self</i>, and returns the portion
  # deleted. The forms that take a <code>Fixnum</code> will raise an
  # <code>IndexError</code> if the value is out of range; the <code>Range</code>
  # form will raise a <code>RangeError</code>, and the <code>Regexp</code> and
  # <code>String</code> forms will silently ignore the assignment.
  #
  #   string = "this is a string"
  #   string.slice!(2)        #=> 105
  #   string.slice!(3..6)     #=> " is "
  #   string.slice!(/s.*t/)   #=> "sa st"
  #   string.slice!("r")      #=> "r"
  #   string                  #=> "thing"
  def slice!(one, two=undefined)
    # This is un-DRY, but it's a simple manual argument splitting. Keeps
    # the code fast and clean since the sequence are pretty short.
    #
    if two.equal?(undefined)
      result = slice(one)

      if one.kind_of? Regexp
        lm = Regexp.last_match
        self[one] = '' if result
        Regexp.last_match = lm
      else
        self[one] = '' if result
      end
    else
      result = slice(one, two)

      if one.kind_of? Regexp
        lm = Regexp.last_match
        self[one, two] = '' if result
        Regexp.last_match = lm
      else
        self[one, two] = '' if result
      end
    end

    result
  end

  # Divides <i>self</i> into substrings based on a delimiter, returning an array
  # of these substrings.
  #
  # If <i>pattern</i> is a <code>String</code>, then its contents are used as
  # the delimiter when splitting <i>self</i>. If <i>pattern</i> is a single
  # space, <i>self</i> is split on whitespace, with leading whitespace and runs
  # of contiguous whitespace characters ignored.
  #
  # If <i>pattern</i> is a <code>Regexp</code>, <i>self</i> is divided where the
  # pattern matches. Whenever the pattern matches a zero-length string,
  # <i>self</i> is split into individual characters.
  #
  # If <i>pattern</i> is omitted, the value of <code>$;</code> is used.  If
  # <code>$;</code> is <code>nil</code> (which is the default), <i>self</i> is
  # split on whitespace as if ` ' were specified.
  #
  # If the <i>limit</i> parameter is omitted, trailing null fields are
  # suppressed. If <i>limit</i> is a positive number, at most that number of
  # fields will be returned (if <i>limit</i> is <code>1</code>, the entire
  # string is returned as the only entry in an array). If negative, there is no
  # limit to the number of fields returned, and trailing null fields are not
  # suppressed.
  #
  #   " now's  the time".split        #=> ["now's", "the", "time"]
  #   " now's  the time".split(' ')   #=> ["now's", "the", "time"]
  #   " now's  the time".split(/ /)   #=> ["", "now's", "", "the", "time"]
  #   "1, 2.34,56, 7".split(%r{,\s*}) #=> ["1", "2.34", "56", "7"]
  #   "hello".split(//)               #=> ["h", "e", "l", "l", "o"]
  #   "hello".split(//, 3)            #=> ["h", "e", "llo"]
  #   "hi mom".split(%r{\s*})         #=> ["h", "i", "m", "o", "m"]
  #
  #   "mellow yellow".split("ello")   #=> ["m", "w y", "w"]
  #   "1,2,,3,4,,".split(',')         #=> ["1", "2", "", "3", "4"]
  #   "1,2,,3,4,,".split(',', 4)      #=> ["1", "2", "", "3,4,,"]
  #   "1,2,,3,4,,".split(',', -4)     #=> ["1", "2", "", "3", "4", "", ""]
  def split(pattern=nil, limit=undefined)

    # Odd edge case
    return [] if empty?

    tail_empty = false

    if limit.equal?(undefined)
      limited = false
    else
      limit = Rubinius::Type.coerce_to limit, Fixnum, :to_int

      if limit > 0
        return [self.dup] if limit == 1
        limited = true
      else
        tail_empty = true
        limited = false
      end
    end

    pattern ||= ($; || " ")

    if pattern == ' '
      if limited
        lim = limit
      elsif tail_empty
        lim = -1
      else
        lim = 0
      end

      return Rubinius.invoke_primitive :string_awk_split, self, lim
    elsif pattern.nil?
      pattern = /\s+/
    elsif pattern.kind_of?(Regexp)
      # Pass
    else
      pattern = StringValue(pattern) unless pattern.kind_of?(String)

      if !limited and limit.equal?(undefined)
        if pattern.empty?
          ret = []
          pos = 0

          while pos < @num_bytes
            ret << substring(pos, 1)
            pos += 1
          end

          return ret
        else
          return split_on_string(pattern)
        end
      end

      pattern = Regexp.new(Regexp.quote(pattern))
    end

    start = 0
    ret = []

    # Handle // as a special case.
    if pattern.source.empty?
      kcode = $KCODE

      begin
        if pattern.options and kc = pattern.kcode
          $KCODE = kc
        end

        if limited
          iterations = limit - 1
          while c = self.find_character(start)
            ret << c
            start += c.size
            iterations -= 1

            break if iterations == 0
          end

          ret << self[start..-1]
        else
          while c = self.find_character(start)
            ret << c
            start += c.size
          end

          # Use #substring because it returns the right class and taints
          # automatically. This is just appending a "", which is this
          # strange protocol if a negative limit is passed in
          ret << substring(0,0) if tail_empty
        end
      ensure
        $KCODE = kcode
      end

      return ret
    end

    last_match = nil

    while match = pattern.match_from(self, start)
      break if limited && limit - ret.size <= 1

      collapsed = match.collapsing?

      unless collapsed && (match.begin(0) == 0)
        ret << match.pre_match_from(last_match ? last_match.end(0) : 0)
        ret.push(*match.captures.compact)
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
      ret << dup
    end

    # Trim from end
    if !ret.empty? and (limit.equal?(undefined) || limit == 0)
      while s = ret.last and s.empty?
        ret.pop
      end
    end

    ret
  end

  def split_on_string(pattern)
    pos = 0

    ret = []

    pat_size = pattern.size

    while pos < @num_bytes
      nxt = find_string(pattern, pos)
      break unless nxt

      match_size = nxt - pos
      ret << substring(pos, match_size)

      pos = nxt + pat_size
    end

    # No more separators, but we need to grab the last part still.
    ret << substring(pos, @num_bytes - pos)

    ret.pop while !ret.empty? and ret.last.empty?

    ret
  end

  # Builds a set of characters from the <i>*strings</i> parameter(s) using the
  # procedure described for <code>String#count</code>. Returns a new string
  # where runs of the same character that occur in this set are replaced by a
  # single character. If no arguments are given, all runs of identical
  # characters are replaced by a single character.
  #
  #   "yellow moon".squeeze                  #=> "yelow mon"
  #   "  now   is  the".squeeze(" ")         #=> " now is the"
  #   "putters shoot balls".squeeze("m-z")   #=> "puters shot balls"
  def squeeze(*strings)
    str = dup
    str.squeeze!(*strings) || str
  end

  # Squeezes <i>self</i> in place, returning either <i>self</i>, or
  # <code>nil</code> if no changes were made.
  def squeeze!(*strings)
    return if @num_bytes == 0
    self.modify!

    table = count_table(*strings).__data__

    i, j, last = 1, 0, @data[0]
    while i < @num_bytes
      c = @data[i]
      unless c == last and table[c] == 1
        @data[j+=1] = last = c
      end
      i += 1
    end

    if (j += 1) < @num_bytes
      @num_bytes = j
      self
    else
      nil
    end
  end

  def start_with?(*prefixes)
    prefixes.each do |prefix|
      prefix = Rubinius::Type.try_convert prefix, String, :to_str
      next unless prefix
      return true if self[0, prefix.length] == prefix
    end
    false
  end

  # Returns a copy of <i>self</i> with leading and trailing whitespace removed.
  #
  #   "    hello    ".strip   #=> "hello"
  #   "\tgoodbye\r\n".strip   #=> "goodbye"
  def strip
    str = dup
    str.strip! || str
  end

  # Removes leading and trailing whitespace from <i>self</i>. Returns
  # <code>nil</code> if <i>self</i> was not altered.
  def strip!
    left = lstrip!
    right = rstrip!
    left.nil? && right.nil? ? nil : self
  end

  # Returns a copy of <i>self</i> with the <em>first</em> occurrence of
  # <i>pattern</i> replaced with either <i>replacement</i> or the value of the
  # block. The <i>pattern</i> will typically be a <code>Regexp</code>; if it is
  # a <code>String</code> then no regular expression metacharacters will be
  # interpreted (that is <code>/\d/</code> will match a digit, but
  # <code>'\d'</code> will match a backslash followed by a 'd').
  #
  # If the method call specifies <i>replacement</i>, special variables such as
  # <code>$&</code> will not be useful, as substitution into the string occurs
  # before the pattern match starts. However, the sequences <code>\1</code>,
  # <code>\2</code>, etc., may be used.
  #
  # In the block form, the current match string is passed in as a parameter, and
  # variables such as <code>$1</code>, <code>$2</code>, <code>$`</code>,
  # <code>$&</code>, and <code>$'</code> will be set appropriately. The value
  # returned by the block will be substituted for the match on each call.
  #
  # The result inherits any tainting in the original string or any supplied
  # replacement string.
  #
  #   "hello".sub(/[aeiou]/, '*')               #=> "h*llo"
  #   "hello".sub(/([aeiou])/, '<\1>')          #=> "h<e>llo"
  #   "hello".sub(/./) {|s| s[0].to_s + ' ' }   #=> "104 ello"
  def sub(pattern, replacement=undefined)
    if replacement.equal?(undefined) and !block_given?
      raise ArgumentError, "wrong number of arguments (1 for 2)"
    end

    unless pattern
      raise ArgumentError, "wrong number of arguments (0 for 2)"
    end

    if match = get_pattern(pattern, true).match_from(self, 0)
      out = match.pre_match

      Regexp.last_match = match

      if replacement.equal?(undefined)
        replacement = yield(match[0].dup).to_s
        out.taint if replacement.tainted?
      else
        out.taint if replacement.tainted?
        replacement = StringValue(replacement).to_sub_replacement(match)
      end

      # We have to reset it again to match the specs
      Regexp.last_match = match

      out << replacement << match.post_match
      out.taint if self.tainted?
    else
      out = self
      Regexp.last_match = nil
    end

    # MRI behavior emulation. Sub'ing String subclasses doen't return the
    # subclass, they return String instances.
    unless instance_of?(String)
      # Do this instead of using self.class.new because some code
      # (ActiveModel) redefines initialize and breaks it.
      Rubinius::Unsafe.set_class out, self.class
    end

    return out
  end

  # Performs the substitutions of <code>String#sub</code> in place,
  # returning <i>self</i>, or <code>nil</code> if no substitutions were
  # performed.
  def sub!(pattern, replacement=undefined)
    # Copied mostly from sub to keep Regexp.last_match= working right.

    if replacement.equal?(undefined) and !block_given?
      raise ArgumentError, "wrong number of arguments (1 for 2)"
    end

    unless pattern
      raise ArgumentError, "wrong number of arguments (0 for 2)"
    end

    if match = get_pattern(pattern, true).match_from(self, 0)
      out = match.pre_match

      Regexp.last_match = match

      if replacement.equal?(undefined)
        replacement = yield(match[0].dup).to_s
        out.taint if replacement.tainted?
      else
        out.taint if replacement.tainted?
        replacement = StringValue(replacement).to_sub_replacement(match)
      end

      # We have to reset it again to match the specs
      Regexp.last_match = match

      out << replacement << match.post_match
      out.taint if self.tainted?
    else
      out = self
      Regexp.last_match = nil
      return nil
    end

    replace(out)

    return self
  end

  # Returns the successor to <i>self</i>. The successor is calculated by
  # incrementing characters starting from the rightmost alphanumeric (or
  # the rightmost character if there are no alphanumerics) in the
  # string. Incrementing a digit always results in another digit, and
  # incrementing a letter results in another letter of the same case.
  # Incrementing nonalphanumerics uses the underlying character set's
  # collating sequence.
  #
  # If the increment generates a ``carry,'' the character to the left of
  # it is incremented. This process repeats until there is no carry,
  # adding an additional character if necessary.
  #
  #   "abcd".succ        #=> "abce"
  #   "THX1138".succ     #=> "THX1139"
  #   "<<koala>>".succ   #=> "<<koalb>>"
  #   "1999zzz".succ     #=> "2000aaa"
  #   "ZZZ9999".succ     #=> "AAAA0000"
  #   "***".succ         #=> "**+"
  def succ
    dup.succ!
  end

  # Equivalent to <code>String#succ</code>, but modifies the receiver in
  # place.
  def succ!
    self.modify!

    return self if @num_bytes == 0

    carry = nil
    last_alnum = 0
    start = @num_bytes - 1

    ctype = Rubinius::CType

    while start >= 0
      s = @data[start]
      if ctype.isalnum(s)
        carry = 0
        if (?0 <= s && s < ?9) ||
           (?a <= s && s < ?z) ||
           (?A <= s && s < ?Z)
          @data[start] += 1
        elsif s == ?9
          @data[start] = ?0
          carry = ?1
        elsif s == ?z
          @data[start] = carry = ?a
        elsif s == ?Z
          @data[start] = carry = ?A
        end

        break if carry == 0
        last_alnum = start
      end

      start -= 1
    end

    if carry.nil?
      start = length - 1
      carry = ?\001

      while start >= 0
        if @data[start] >= 255
          @data[start] = 0
        else
          @data[start] += 1
          break
        end

        start -= 1
      end
    end

    if start < 0
      splice! last_alnum, 1, carry.chr + @data[last_alnum].chr
    end

    return self
  end

  alias_method :next, :succ
  alias_method :next!, :succ!

  # Returns a basic <em>n</em>-bit checksum of the characters in <i>self</i>,
  # where <em>n</em> is the optional <code>Fixnum</code> parameter, defaulting
  # to 16. The result is simply the sum of the binary value of each character in
  # <i>self</i> modulo <code>2n - 1</code>. This is not a particularly good
  # checksum.
  def sum(bits=16)
    bits = Rubinius::Type.coerce_to bits, Fixnum, :to_int
    i = -1
    sum = 0

    sum += @data[i] while (i += 1) < @num_bytes
    if bits > 0
      sum & ((1 << bits) - 1)
    else
      sum
    end
  end

  # Returns a copy of <i>self</i> with uppercase alphabetic characters converted to
  # lowercase and lowercase characters converted to uppercase.
  #
  #   "Hello".swapcase          #=> "hELLO"
  #   "cYbEr_PuNk11".swapcase   #=> "CyBeR_pUnK11"
  def swapcase
    str = dup
    str.swapcase! || str
  end

  # Equivalent to <code>String#swapcase</code>, but modifies the receiver in
  # place, returning <i>self</i>, or <code>nil</code> if no changes were made.
  def swapcase!
    self.modify!
    return if @num_bytes == 0

    modified = false

    ctype = Rubinius::CType

    i = 0
    while i < @num_bytes
      c = @data[i]
      if ctype.islower(c)
        @data[i] = ctype.toupper!(c)
        modified = true
      elsif ctype.isupper(c)
        @data[i] = ctype.tolower!(c)
        modified = true
      end
      i += 1
    end

    modified ? self : nil
  end

  # The +intern+ method is an alias of +to_sym+. See <code>Symbol#to_sym</code>.
  alias_method :intern, :to_sym

  # Returns the result of interpreting leading characters in <i>self</i> as an
  # integer base <i>base</i> (between 2 and 36). Extraneous characters past the
  # end of a valid number are ignored. If there is not a valid number at the
  # start of <i>self</i>, <code>0</code> is returned. This method never raises an
  # exception.
  #
  #   "12345".to_i             #=> 12345
  #   "99 red balloons".to_i   #=> 99
  #   "0a".to_i                #=> 0
  #   "0a".to_i(16)            #=> 10
  #   "hello".to_i             #=> 0
  #   "1100101".to_i(2)        #=> 101
  #   "1100101".to_i(8)        #=> 294977
  #   "1100101".to_i(10)       #=> 1100101
  #   "1100101".to_i(16)       #=> 17826049
  def to_i(base=10)
    base = Rubinius::Type.coerce_to base, Integer, :to_int

    if base < 0 || base == 1 || base > 36
      raise ArgumentError, "illegal radix #{base}"
    end

    to_inum(base, false)
  end

  # Returns self if self is an instance of String,
  # else returns self converted to a String instance.
  def to_s
    instance_of?(String) ? self : "".replace(self)
  end
  alias_method :to_str, :to_s

  # Returns a copy of <i>self</i> with the characters in <i>from_str</i> replaced
  # by the corresponding characters in <i>to_str</i>. If <i>to_str</i> is
  # shorter than <i>from_str</i>, it is padded with its last character. Both
  # strings may use the c1--c2 notation to denote ranges of characters, and
  # <i>from_str</i> may start with a <code>^</code>, which denotes all
  # characters except those listed.
  #
  #    "hello".tr('aeiou', '*')    #=> "h*ll*"
  #    "hello".tr('^aeiou', '*')   #=> "*e**o"
  #    "hello".tr('el', 'ip')      #=> "hippo"
  #    "hello".tr('a-y', 'b-z')    #=> "ifmmp"
  def tr(source, replacement)
    str = dup
    str.tr!(source, replacement) || str
  end

  # Translates <i>self</i> in place, using the same rules as
  # <code>String#tr</code>. Returns <i>self</i>, or <code>nil</code> if no
  # changes were made.
  def tr!(source, replacement)
    tr_trans(source, replacement, false)
  end

  # Processes a copy of <i>self</i> as described under <code>String#tr</code>,
  # then removes duplicate characters in regions that were affected by the
  # translation.
  #
  #    "hello".tr_s('l', 'r')     #=> "hero"
  #    "hello".tr_s('el', '*')    #=> "h*o"
  #    "hello".tr_s('el', 'hx')   #=> "hhxo"
  def tr_s(source, replacement)
    str = dup
    str.tr_s!(source, replacement) || str
  end

  # Performs <code>String#tr_s</code> processing on <i>self</i> in place,
  # returning <i>self</i>, or <code>nil</code> if no changes were made.
  def tr_s!(source, replacement)
    tr_trans(source, replacement, true)
  end

  # Returns a copy of <i>self</i> with all lowercase letters replaced with their
  # uppercase counterparts. The operation is locale insensitive---only
  # characters ``a'' to ``z'' are affected.
  #
  #   "hEllO".upcase   #=> "HELLO"
  def upcase
    str = dup
    str.upcase! || str
  end

  ##
  # Upcases the contents of <i>self</i>, returning <code>nil</code> if no
  # changes were made.

  def upcase!
    return if @num_bytes == 0
    self.modify!

    modified = false

    ctype = Rubinius::CType

    i = 0
    while i < @num_bytes
      c = @data[i]
      if ctype.islower(c)
        @data[i] = ctype.toupper!(c)
        modified = true
      end
      i += 1
    end

    modified ? self : nil
  end

  def upto(stop, exclusive=false)
    stop = StringValue(stop)
    return self if self > stop

    after_stop = exclusive ? stop : stop.succ
    current = self

    until current == after_stop
      yield current
      current = StringValue(current.succ)
      break if current.size > stop.size || current.size == 0
    end

    self
  end

  def tr_trans(source, replacement, squeeze)
    source = StringValue(source).dup
    replacement = StringValue(replacement).dup

    self.modify!

    return self.delete!(source) if replacement.empty?
    return if @num_bytes == 0

    invert = source[0] == ?^ && source.length > 1
    expanded = source.tr_expand! nil, true
    size = source.size
    src = source.__data__

    if invert
      replacement.tr_expand! nil, false
      r = replacement.__data__[replacement.size-1]
      table = Rubinius::Tuple.pattern 256, r

      i = 0
      while i < size
        table[src[i]] = -1
        i += 1
      end
    else
      table = Rubinius::Tuple.pattern 256, -1

      replacement.tr_expand! expanded, false
      repl = replacement.__data__
      rsize = replacement.size
      i = 0
      while i < size
        r = repl[i] if i < rsize
        table[src[i]] = r
        i += 1
      end
    end

    self.modify!
    modified = false

    if squeeze
      i, j, last = -1, -1, nil
      while (i += 1) < @num_bytes
        s = @data[i]
        c = table[s]
        if c >= 0
          next if last == c
          @data[j+=1] = last = c
          modified = true
        else
          @data[j+=1] = s
          last = nil
        end
      end

      @num_bytes = j if (j += 1) < @num_bytes
    else
      i = 0
      while i < @num_bytes
        c = table[@data[i]]
        if c >= 0
          @data[i] = c
          modified = true
        end
        i += 1
      end
    end

    return modified ? self : nil
  end

  def to_sub_replacement(match)
    index = 0
    result = ""
    while index < @num_bytes
      current = index
      while current < @num_bytes && @data[current] != ?\\
        current += 1
      end
      result << substring(index, current - index)
      break if current == @num_bytes

      # found backslash escape, looking next
      if current == @num_bytes - 1
        result << ?\\ # backslash at end of string
        break
      end
      index = current + 1

      cap = @data[index]

      result << case cap
                when ?&
                  match[0]
                when ?`
                  match.pre_match
                when ?'
                  match.post_match
                when ?+
                  match.captures.compact[-1].to_s
                when ?0..?9
                  match[cap - ?0].to_s
                when ?\\ # escaped backslash
                  '\\'
                else     # unknown escape
                  '\\' << cap
                end
      index += 1
    end
    return result
  end

  def to_inum(base, check)
    Ruby.primitive :string_to_inum
    raise ArgumentError, "invalid value for Integer"
  end

  def apply_and!(other)
    Ruby.primitive :string_apply_and
    raise PrimitiveFailure, "String#apply_and! primitive failed"
  end

  def compare_substring(other, start, size)
    Ruby.primitive :string_compare_substring

    if start > @num_bytes || start + @num_bytes < 0
      raise IndexError, "index #{start} out of string"
    end
    raise PrimitiveFailure, "String#compare_substring primitive failed"
  end

  def count_table(*strings)
    table = String.pattern 256, 1

    i, size = 0, strings.size
    while i < size
      str = StringValue(strings[i]).dup
      if str.size > 1 && str[0] == ?^
        pos, neg = 0, 1
      else
        pos, neg = 1, 0
      end

      set = String.pattern 256, neg
      str.tr_expand! nil, true
      j, chars = -1, str.size
      set[str[j]] = pos while (j += 1) < chars

      table.apply_and! set
      i += 1
    end
    table
  end

  def tr_expand!(limit, invalid_as_empty)
    Ruby.primitive :string_tr_expand
    raise PrimitiveFailure, "String#tr_expand primitive failed"
  end

  def justify(width, direction, padstr=" ")
    padstr = StringValue(padstr)
    raise ArgumentError, "zero width padding" if padstr.size == 0

    width = Rubinius::Type.coerce_to width, Fixnum, :to_int
    if width > @num_bytes
      padsize = width - @num_bytes
    else
      return dup
    end

    str = self.class.new("\0") * (padsize + @num_bytes)
    str.taint if tainted? or padstr.tainted?

    case direction
    when :right
      pad = String.pattern padsize, padstr
      str.copy_from pad, 0, padsize, 0
      str.copy_from self, 0, @num_bytes, padsize
    when :left
      pad = String.pattern padsize, padstr
      str.copy_from self, 0, @num_bytes, 0
      str.copy_from pad, 0, padsize, @num_bytes
    when :center
      half = padsize / 2.0
      lsize = half.floor
      rsize = half.ceil
      lpad = String.pattern lsize, padstr
      rpad = String.pattern rsize, padstr
      str.copy_from lpad, 0, lsize, 0
      str.copy_from self, 0, @num_bytes, lsize
      str.copy_from rpad, 0, rsize, lsize + @num_bytes
    end

    str
  end

  # Unshares shared strings.
  def modify!
    Ruby.check_frozen

    if @shared
      @data = @data.dup
      @shared = nil
    end

    @hash_value = nil # reset the hash value
  end

  def subpattern(pattern, capture)
    # TODO: A part of the functionality here should go into MatchData#[]
    match = pattern.match(self)
    if !match or capture >= match.size
      return nil
    end

    if capture < 0
      capture += match.size
      return nil if capture <= 0
    end

    start = match.begin(capture)
    count = match.end(capture) - match.begin(capture)
    str = self.substring(start, count)
    str.taint if pattern.tainted?
    [match, str]
  end

  def subpattern_set(pattern, capture, replacement)
    unless match = pattern.match(self)
      raise IndexError, "regexp not matched"
    end

    raise IndexError, "index #{capture} out of regexp" if capture >= match.size

    if capture < 0
      raise IndexError, "index #{capture} out of regexp" if -capture >= match.size
      capture += match.size
    end

    start  = match.begin(capture)
    length = match.end(capture) - start
    splice! start, length, replacement
  end

  def splice!(start, count, replacement)
    if start < 0
      start += @num_bytes

      if start < 0 or start > @num_bytes
        raise IndexError, "index #{start} out of string"
      end
    elsif start > @num_bytes
      raise IndexError, "index #{start} out of string"
    end

    raise IndexError, "negative length #{count}" if count < 0

    modify!

    replacement = StringValue replacement

    # Clamp count to the end of the string
    count = @num_bytes - start if start + count > @num_bytes

    rsize = replacement.size

    if rsize == 0
      trailer_start = start + count
      trailer_size =  @num_bytes - trailer_start

      copy_from self, trailer_start, trailer_size, start
      @num_bytes -= count
    else
      # Resize if necessary
      new_size = @num_bytes - count + rsize
      resize_capacity new_size if new_size > @data.size

      # easy, fits right in.
      if count == rsize
        copy_from replacement, 0, rsize, start
      else
        # shift the bytes on the end in or out
        trailer_start = start + count
        trailer_size =  @num_bytes - trailer_start

        copy_from self, trailer_start, trailer_size, start + rsize

        # Then put the replacement in
        copy_from replacement, 0, rsize, start

        @num_bytes += (rsize - count)
      end
    end

    taint if replacement.tainted?

    self
  end

  def prefix?(other)
    size = other.size
    return false if size > @num_bytes
    other.compare_substring(self, 0, size) == 0
  end

  def suffix?(other)
    size = other.size
    return false if size > @num_bytes
    other.compare_substring(self, -size, size) == 0
  end

  def shorten!(size)
    self.modify!
    return if @num_bytes == 0
    @num_bytes = @characters = @num_bytes - size
  end

  def dump
    str = self.class.new '"'
    str << transform(Rubinius::CType::Printed, false)
    str << '"'
    str
  end

  def shared!
    @shared = true
  end

  def get_pattern(pattern, quote=false)
    case pattern
    when Regexp
      return pattern
    when String
      # nothing
    else
      pattern = StringValue(pattern)
    end

    pattern = Regexp.quote(pattern) if quote
    Regexp.new(pattern)
  end

  ##
  #  call-seq:
  #     str.unpack(format)   => anArray
  #
  #  Decodes <i>str</i> (which may contain binary data) according to
  #  the format string, returning an array of each value
  #  extracted. The format string consists of a sequence of
  #  single-character directives, summarized in the table at the end
  #  of this entry.
  #
  #  Each directive may be followed by a number, indicating the number
  #  of times to repeat with this directive. An asterisk
  #  (``<code>*</code>'') will use up all remaining elements. The
  #  directives <code>sSiIlL</code> may each be followed by an
  #  underscore (``<code>_</code>'') to use the underlying platform's
  #  native size for the specified type; otherwise, it uses a
  #  platform-independent consistent size. Spaces are ignored in the
  #  format string. See also <code>Array#pack</code>.
  #
  #     "abc \0\0abc \0\0".unpack('A6Z6')   #=> ["abc", "abc "]
  #     "abc \0\0".unpack('a3a3')           #=> ["abc", " \000\000"]
  #     "abc \0abc \0".unpack('Z*Z*')       #=> ["abc ", "abc "]
  #     "aa".unpack('b8B8')                 #=> ["10000110", "01100001"]
  #     "aaa".unpack('h2H2c')               #=> ["16", "61", 97]
  #     "\xfe\xff\xfe\xff".unpack('sS')     #=> [-2, 65534]
  #     "now=20is".unpack('M*')             #=> ["now is"]
  #     "whole".unpack('xax2aX2aX1aX2a')    #=> ["h", "e", "l", "l", "o"]
  #
  #  This table summarizes the various formats and the Ruby classes
  #  returned by each.
  #
  #     Format | Returns | Function
  #     -------+---------+-----------------------------------------
  #       A    | String  | with trailing nulls and spaces removed
  #     -------+---------+-----------------------------------------
  #       a    | String  | string
  #     -------+---------+-----------------------------------------
  #       B    | String  | extract bits from each character (msb first)
  #     -------+---------+-----------------------------------------
  #       b    | String  | extract bits from each character (lsb first)
  #     -------+---------+-----------------------------------------
  #       C    | Fixnum  | extract a character as an unsigned integer
  #     -------+---------+-----------------------------------------
  #       c    | Fixnum  | extract a character as an integer
  #     -------+---------+-----------------------------------------
  #       d,D  | Float   | treat sizeof(double) characters as
  #            |         | a native double
  #     -------+---------+-----------------------------------------
  #       E    | Float   | treat sizeof(double) characters as
  #            |         | a double in little-endian byte order
  #     -------+---------+-----------------------------------------
  #       e    | Float   | treat sizeof(float) characters as
  #            |         | a float in little-endian byte order
  #     -------+---------+-----------------------------------------
  #       f,F  | Float   | treat sizeof(float) characters as
  #            |         | a native float
  #     -------+---------+-----------------------------------------
  #       G    | Float   | treat sizeof(double) characters as
  #            |         | a double in network byte order
  #     -------+---------+-----------------------------------------
  #       g    | Float   | treat sizeof(float) characters as a
  #            |         | float in network byte order
  #     -------+---------+-----------------------------------------
  #       H    | String  | extract hex nibbles from each character
  #            |         | (most significant first)
  #     -------+---------+-----------------------------------------
  #       h    | String  | extract hex nibbles from each character
  #            |         | (least significant first)
  #     -------+---------+-----------------------------------------
  #       I    | Integer | treat sizeof(int) (modified by _)
  #            |         | successive characters as an unsigned
  #            |         | native integer
  #     -------+---------+-----------------------------------------
  #       i    | Integer | treat sizeof(int) (modified by _)
  #            |         | successive characters as a signed
  #            |         | native integer
  #     -------+---------+-----------------------------------------
  #       L    | Integer | treat four (modified by _) successive
  #            |         | characters as an unsigned native
  #            |         | long integer
  #     -------+---------+-----------------------------------------
  #       l    | Integer | treat four (modified by _) successive
  #            |         | characters as a signed native
  #            |         | long integer
  #     -------+---------+-----------------------------------------
  #       M    | String  | quoted-printable
  #     -------+---------+-----------------------------------------
  #       m    | String  | base64-encoded
  #     -------+---------+-----------------------------------------
  #       N    | Integer | treat four characters as an unsigned
  #            |         | long in network byte order
  #     -------+---------+-----------------------------------------
  #       n    | Fixnum  | treat two characters as an unsigned
  #            |         | short in network byte order
  #     -------+---------+-----------------------------------------
  #       P    | String  | treat sizeof(char *) characters as a
  #            |         | pointer, and  return \emph{len} characters
  #            |         | from the referenced location
  #     -------+---------+-----------------------------------------
  #       p    | String  | treat sizeof(char *) characters as a
  #            |         | pointer to a  null-terminated string
  #     -------+---------+-----------------------------------------
  #       Q    | Integer | treat 8 characters as an unsigned
  #            |         | quad word (64 bits)
  #     -------+---------+-----------------------------------------
  #       q    | Integer | treat 8 characters as a signed
  #            |         | quad word (64 bits)
  #     -------+---------+-----------------------------------------
  #       S    | Fixnum  | treat two (different if _ used)
  #            |         | successive characters as an unsigned
  #            |         | short in native byte order
  #     -------+---------+-----------------------------------------
  #       s    | Fixnum  | Treat two (different if _ used)
  #            |         | successive characters as a signed short
  #            |         | in native byte order
  #     -------+---------+-----------------------------------------
  #       U    | Integer | UTF-8 characters as unsigned integers
  #     -------+---------+-----------------------------------------
  #       u    | String  | UU-encoded
  #     -------+---------+-----------------------------------------
  #       V    | Fixnum  | treat four characters as an unsigned
  #            |         | long in little-endian byte order
  #     -------+---------+-----------------------------------------
  #       v    | Fixnum  | treat two characters as an unsigned
  #            |         | short in little-endian byte order
  #     -------+---------+-----------------------------------------
  #       w    | Integer | BER-compressed integer (see Array.pack)
  #     -------+---------+-----------------------------------------
  #       X    | ---     | skip backward one character
  #     -------+---------+-----------------------------------------
  #       x    | ---     | skip forward one character
  #     -------+---------+-----------------------------------------
  #       Z    | String  | with trailing nulls removed
  #            |         | upto first null with *
  #     -------+---------+-----------------------------------------
  #       @    | ---     | skip to the offset given by the
  #            |         | length argument
  #     -------+---------+-----------------------------------------

  def unpack(directives)
    Ruby.primitive :string_unpack

    unless directives.kind_of? String
      return unpack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
  end
end

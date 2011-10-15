class String
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, String, :to_str
  end

  def initialize(arg = undefined)
    Rubinius.check_frozen
    replace StringValue(arg) unless arg.equal?(undefined)

    self
  end

  private :initialize

  def encode(to=undefined, from=undefined, options=nil)
    # TODO
    self
  end
  
  def hex
    return 0 if self.chars.first == "_"
    to_inum(16, false)
  end

  def partition(pattern)
    unless pattern.is_a? Regexp
      pattern = Rubinius::Type.coerce_to(pattern, String, :to_str)
    end
    i = index(pattern)
    return [self, "", ""] unless i

    if pattern.is_a? Regexp
      match = Regexp.last_match
      [match.pre_match, match[0], match.post_match]
    else
      last = i+pattern.length
      [self[0...i], self[i...last], self[last...length]]
    end
  end

  def upto(stop, exclusive=false)
    return to_enum :upto, stop, exclusive unless block_given?
    stop = StringValue(stop)
    return self if self > stop

    if stop.size == 1 && size == 1
      after_stop = stop.getbyte(0) + (exclusive ? 0 : 1)
      current = getbyte(0)
      until current == after_stop
        yield current.chr
        current += 1
      end
    else
      after_stop = exclusive ? stop : stop.succ
      current = self

      until current == after_stop
        yield current
        current = StringValue(current.succ)
        break if current.size > stop.size || current.size == 0
      end
    end
    self
  end

  def ord
    raise ArgumentError, 'empty string' if empty?
    @data[0]
  end

  def encoding
    # TODO: temporary until encoding is set on create.
    @encoding ||= Encoding.find("us-ascii")
  end

  def force_encoding(name)
    self
  end

  # Reverses <i>self</i> in place.
  def reverse!
    Rubinius.check_frozen

    return self if @num_bytes <= 1
    self.modify!

    @data.reverse(0, @num_bytes)
    self
  end

  # Squeezes <i>self</i> in place, returning either <i>self</i>, or
  # <code>nil</code> if no changes were made.
  def squeeze!(*strings)
    if strings.first =~ /.+\-.+/
      range = strings.first.gsub(/-/, '').split('')
      raise ArgumentError, "invalid range #{strings} in string transliteration" unless range == range.sort
    end

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

  # Performs the substitutions of <code>String#sub</code> in place,
  # returning <i>self</i>, or <code>nil</code> if no substitutions were
  # performed.
  #
  def sub!(pattern, replacement=undefined)
    # Copied mostly from sub to keep Regexp.last_match= working right.

    if replacement.equal?(undefined) and !block_given?
      raise ArgumentError, "wrong number of arguments (1 for 2)"
    end

    unless pattern
      raise ArgumentError, "wrong number of arguments (0 for 2)"
    end

    Rubinius.check_frozen

    if match = get_pattern(pattern, true).match_from(self, 0)
      out = match.pre_match

      Regexp.last_match = match

      if replacement.equal?(undefined)
        replacement = yield(match[0].dup).to_s
        out.taint if replacement.tainted?
        out.append(replacement).append(match.post_match)
      else
        out.taint if replacement.tainted?
        replacement = StringValue(replacement).to_sub_replacement(out, match)
        out.append(match.post_match)
      end

      # We have to reset it again to match the specs
      Regexp.last_match = match

      out.taint if self.tainted?
    else
      out = self
      Regexp.last_match = nil
      return nil
    end

    replace(out)

    return self
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
    Rubinius.check_frozen
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

  # Equivalent to <code>String#succ</code>, but modifies the receiver in
  # place.
  #
  # TODO: make encoding aware.
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
        if (48 <= s && s < 57) ||
           (97 <= s && s < 122) ||
           (65 <= s && s < 90)
          @data[start] += 1
        elsif s == 57
          @data[start] = 48
          carry = 49
        elsif s == 122
          @data[start] = carry = 97
        elsif s == 90
          @data[start] = carry = 65
        end

        break if carry == 0
        last_alnum = start
      end

      start -= 1
    end

    if carry.nil?
      start = length - 1
      carry = 1

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
  
  def to_r
    return Rational(0,1) if empty?
    
    clean       = strip
    numerator   = clean.gsub(".", "").to_i
    denominator = 1
    
    if clean.match(/\d\.\d/)
      denominator = 10**clean.split(".")[1].to_i.to_s.length
    end

    if clean.match(/\d\/\d/)
      denominator *= clean.split("/")[1].to_i
    end
    
    return Rational(numerator, denominator)
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
    Rubinius.primitive :string_unpack19

    unless directives.kind_of? String
      return unpack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
  end

  # Removes trailing whitespace from <i>self</i>, returning <code>nil</code> if
  # no change was made. See also <code>String#lstrip!</code> and
  # <code>String#strip!</code>.
  #
  #   "  hello  ".rstrip   #=> "  hello"
  #   "hello".rstrip!      #=> nil
  def rstrip!
    Rubinius.check_frozen
    return if @num_bytes == 0

    stop = @num_bytes - 1

    ctype = Rubinius::CType

    while stop >= 0 && (@data[stop] == 0 || ctype.isspace(@data[stop]))
      stop -= 1
    end

    return if (stop += 1) == @num_bytes

    modify!
    @num_bytes = stop
    self
  end

  # Removes leading whitespace from <i>self</i>, returning <code>nil</code> if no
  # change was made. See also <code>String#rstrip!</code> and
  # <code>String#strip!</code>.
  #
  #   "  hello  ".lstrip   #=> "hello  "
  #   "hello".lstrip!      #=> nil
  def lstrip!
    Rubinius.check_frozen
    return if @num_bytes == 0

    start = 0

    ctype = Rubinius::CType

    while start < @num_bytes && ctype.isspace(@data[start])
      start += 1
    end

    return if start == 0

    modify!
    @num_bytes = @num_bytes - start
    @data.move_bytes start, @num_bytes, 0
    self
  end

  # Processes <i>self</i> as for <code>String#chop</code>, returning <i>self</i>,
  # or <code>nil</code> if <i>self</i> is the empty string.  See also
  # <code>String#chomp!</code>.
  def chop!
    Rubinius.check_frozen
    return if @num_bytes == 0

    self.modify!

    if @num_bytes > 1 and
        @data[@num_bytes-1] == 10 and @data[@num_bytes-2] == 13
      @num_bytes = @num_bytes - 2
    else
      @num_bytes = @num_bytes - 1
    end

    self
  end

  # Modifies <i>self</i> in place as described for <code>String#chomp</code>,
  # returning <i>self</i>, or <code>nil</code> if no modifications were made.
  #---
  # NOTE: TypeError is raised in String#replace and not in String#chomp! when
  # self is frozen. This is intended behaviour.
  #+++
  def chomp!(sep=undefined)
    Rubinius.check_frozen

    # special case for performance. No seperator is by far the most common usage.
    if sep.equal?(undefined)
      return if @num_bytes == 0

      c = @data[@num_bytes-1]
      if c == 10 # ?\n
        @num_bytes -= 1 if @num_bytes > 1 && @data[@num_bytes-2] == 13 # ?\r
      elsif c != 13 # ?\r
        return
      end

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = @num_bytes - 1
      return self
    end

    return if sep.nil? || @num_bytes == 0
    sep = StringValue sep

    if (sep == $/ && sep == DEFAULT_RECORD_SEPARATOR) || sep == "\n"
      c = @data[@num_bytes-1]
      if c == 10 # ?\n
        @num_bytes -= 1 if @num_bytes > 1 && @data[@num_bytes-2] == 13 # ?\r
      elsif c != 13 # ?\r
        return
      end

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = @num_bytes - 1
    elsif sep.size == 0
      size = @num_bytes
      while size > 0 && @data[size-1] == 10 # ?\n
        if size > 1 && @data[size-2] == 13 # ?\r
          size -= 2
        else
          size -= 1
        end
      end

      return if size == @num_bytes

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = size
    else
      size = sep.size
      return if size > @num_bytes || sep.compare_substring(self, -size, size) != 0

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      @num_bytes = @num_bytes - size
    end

    return self
  end

  # Replaces the contents and taintedness of <i>string</i> with the corresponding
  # values in <i>other</i>.
  #
  #   s = "hello"         #=> "hello"
  #   s.replace "world"   #=> "world"
  def replace(other)
    Rubinius.check_frozen

    # If we're replacing with ourselves, then we have nothing to do
    return self if equal?(other)

    other = StringValue(other)

    @shared = true
    other.shared!
    @data = other.__data__
    @num_bytes = other.num_bytes
    @hash_value = nil

    taint if other.tainted?

    self
  end
  alias_method :initialize_copy, :replace
  # private :initialize_copy

  # Returns a new string with the characters from <i>self</i> in reverse order.
  #
  #   "stressed".reverse   #=> "desserts"
end

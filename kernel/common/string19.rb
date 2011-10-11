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
end

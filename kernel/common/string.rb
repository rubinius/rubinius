# -*- encoding: us-ascii -*-

# Default Ruby Record Separator
# Used in this file and by various methods that need to ignore $/
DEFAULT_RECORD_SEPARATOR = "\n"

class String
  include Comparable

  def self.allocate
    str = super()
    str.__data__ = Rubinius::ByteArray.new(1)
    str.num_bytes = 0
    str
  end

  attr_accessor :data

  alias_method :__data__, :data
  alias_method :__data__=, :data=

  ##
  # Creates a new string from copying _count_ bytes from the
  # _start_ of _bytes_.
  def self.from_bytearray(bytes, start, count)
    Rubinius.primitive :string_from_bytearray
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

  def %(args)
    if args.respond_to?(:to_ary)
      ret = Rubinius::Sprinter.get(self).call(*args)
    else
      ret = Rubinius::Sprinter.get(self).call(args)
    end

    ret.taint if tainted?
    return ret
  end

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

  def +(other)
    other = StringValue(other)
    Rubinius::Type.check_encoding_compatible self, other
    String.new(self) << other
  end

  def <=>(other)
    if other.kind_of?(String)
      @data.compare_bytes(other.__data__, @num_bytes, other.bytesize)
    else
      return unless other.respond_to?(:to_str) && other.respond_to?(:<=>)
      return unless tmp = (other <=> self)
      return -tmp # We're not supposed to convert to integer here
    end
  end

  def ==(other)
    Rubinius.primitive :string_equal

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

  def [](index, other = undefined)
    Rubinius.primitive :string_aref

    unless other.equal?(undefined)
      if index.kind_of? Regexp
        match, str = subpattern(index, other)
        Regexp.last_match = match
        return str
      else
        length = Rubinius::Type.coerce_to(other, Fixnum, :to_int)
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

      start += size if start < 0

      length += size if length < 0
      length += 1 unless index.exclude_end?

      return "" if start == size
      return nil if start < 0 || start > size

      length = size if length > size
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
      return self[index]
    end
  end
  alias_method :slice, :[]

  def capitalize
    return dup if @num_bytes == 0

    str = transform(Rubinius::CType::Lowered, true)

    str.modify!

    # Now do the actual capitalization
    ba = str.__data__
    ba[0] = Rubinius::CType.toupper(ba[0])

    return str
  end

  def capitalize!
    Rubinius.check_frozen

    cap = capitalize()
    return nil if cap == self

    replace(cap)
    return self
  end

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

  def center(width, padstr = " ")
    justify width, :center, padstr
  end

  def chomp(separator=$/)
    str = dup
    str.chomp!(separator) || str
  end

  def chop
    str = dup
    str.chop! || str
  end

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

  def crypt(other_str)
    other_str = StringValue(other_str)

    if other_str.size < 2
      raise ArgumentError, "salt must be at least 2 characters"
    end

    hash = __crypt__(other_str)
    hash.taint if tainted? || other_str.tainted?
    hash
  end

  def delete(*strings)
    str = dup
    str.delete!(*strings) || str
  end

  def delete!(*strings)
    raise ArgumentError, "wrong number of arguments" if strings.empty?

    table = count_table(*strings).__data__

    self.modify!

    i, j = 0, -1
    while i < @num_bytes
      c = @data[i]
      unless table[c] == 1
        @data[j+=1] = c
      end
      i += 1
    end

    if (j += 1) < @num_bytes
      self.num_bytes = j
      self
    else
      nil
    end
  end

  def downcase
    return dup if @num_bytes == 0
    transform(Rubinius::CType::Lowered, true)
  end

  def downcase!
    Rubinius.check_frozen

    return if @num_bytes == 0

    str = transform(Rubinius::CType::Lowered, true)

    return nil if str == self

    replace(str)

    return self
  end

  def chars(&block)
    return to_enum :chars unless block_given?
    # TODO: Use Encodings for KCODE in 1.8 mode
    return scan(/./u, &block) if Rubinius.kcode == :UTF8 and Rubinius.ruby18?

    i = 0
    n = size
    while i < n
      yield substring(i, 1)
      i += 1
    end

    self
  end

  alias_method :each_char, :chars

  def bytes
    return to_enum :bytes unless block_given?
    i = 0
    while i < @num_bytes do
      yield @data.get_byte(i)
      i += 1
    end
    self
  end

  alias_method :each_byte, :bytes

  def empty?
    @num_bytes == 0
  end

  def end_with?(*suffixes)
    suffixes.each do |suffix|
      suffix = Rubinius::Type.check_convert_type suffix, String, :to_str
      next unless suffix

      return true if self[-suffix.length, suffix.length] == suffix
    end
    false
  end

  def eql?(other)
    Rubinius.primitive :string_equal

    return false unless other.kind_of?(String) && other.size == @num_bytes
    return @data.compare_bytes(other.__data__, @num_bytes, other.size) == 0
  end

  # This method is specifically part of 1.9 but we enable it in 1.8 also
  # because we need it internally.
  def getbyte(index)
    index = Rubinius::Type.coerce_to index, Fixnum, :to_int

    index += bytesize if index < 0
    return if index < 0 or index >= bytesize

    @data[index]
  end

  def include?(needle)
    if needle.kind_of? Fixnum
      needle = needle % 256
      str_needle = needle.chr
    else
      str_needle = StringValue(needle)
    end

    !!find_string(str_needle, 0)
  end

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

  def insert(index, other)
    other = StringValue(other)
    index = Rubinius::Type.coerce_to index, Fixnum, :to_int

    osize = other.size
    size = @num_bytes + osize
    str = self.class.pattern size, "\0"

    index = @num_bytes + 1 + index if index < 0
    if index > @num_bytes or index < 0 then
      raise IndexError, "index #{index} out of string"
    end

    Rubinius.check_frozen
    hash_value = nil

    if index == @num_bytes
      str.copy_from self, 0, @num_bytes, 0
      str.copy_from other, 0, osize, @num_bytes
    else
      str.copy_from self, 0, index, 0 if index > 0
      str.copy_from other, 0, osize, index
      str.copy_from self, index, @num_bytes - index, index + osize
    end

    self.num_bytes = size
    @data = str.__data__
    taint if other.tainted?

    self
  end

  ControlCharacters = [10, 9, 7, 11, 12, 13, 27, 8]
  ControlPrintValue = ["\\n", "\\t", "\\a", "\\v", "\\f", "\\r", "\\e", "\\b"]

  def ljust(width, padstr=" ")
    justify(width, :left, padstr)
  end

  def lstrip
    str = dup
    str.lstrip! || str
  end

  def oct
    to_inum(-8, false)
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

  def reverse
    dup.reverse!
  end

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

  def partition(pattern=nil)
    return super() if pattern == nil && block_given?

    if pattern.kind_of? Regexp
      if m = pattern.match(self)
        Regexp.last_match = m
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

  def rjust(width, padstr = " ")
    justify(width, :right, padstr)
  end

  def rstrip
    str = dup
    str.rstrip! || str
  end

  def scan(pattern)
    taint = tainted? || pattern.tainted?
    pattern = Rubinius::Type.coerce_to_regexp(pattern, true)
    index = 0

    last_match = nil

    if block_given?
      ret = self
    else
      ret = []
    end

    while match = pattern.match_from(self, index)
      fin = match.full.at(1)

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

  # This method is specifically part of 1.9 but we enable it in 1.8 also
  # because we need it internally.
  def setbyte(index, byte)
    self.modify!

    index = Rubinius::Type.coerce_to index, Fixnum, :to_int
    byte = Rubinius::Type.coerce_to byte, Fixnum, :to_int

    index += bytesize if index < 0
    if index < 0 or index >= bytesize
      raise IndexError, "byte index #{index} is outside bounds of String"
    end

    @ascii_only = @valid_encoding = nil
    @data[index] = byte
  end

  def split(pattern=nil, limit=undefined)
    Rubinius::Splitter.split(self, pattern, limit)
  end

  def squeeze(*strings)
    str = dup
    str.squeeze!(*strings) || str
  end

  def squeeze!(*strings)
    return if @num_bytes == 0

    table = count_table(*strings).__data__
    self.modify!

    i, j, last = 1, 0, @data[0]
    while i < @num_bytes
      c = @data[i]
      unless c == last and table[c] == 1
        @data[j+=1] = last = c
      end
      i += 1
    end

    if (j += 1) < @num_bytes
      self.num_bytes = j
      self
    else
      nil
    end
  end

  def start_with?(*prefixes)
    prefixes.each do |prefix|
      prefix = Rubinius::Type.check_convert_type prefix, String, :to_str
      next unless prefix
      return true if self[0, prefix.length] == prefix
    end
    false
  end

  def strip
    str = dup
    str.strip! || str
  end

  def strip!
    left = lstrip!
    right = rstrip!
    left.nil? && right.nil? ? nil : self
  end

  def sub(pattern, replacement=undefined)
    if replacement.equal?(undefined) and !block_given?
      raise ArgumentError, "wrong number of arguments (1 for 2)"
    end

    unless pattern
      raise ArgumentError, "wrong number of arguments (0 for 2)"
    end

    if match = Rubinius::Type.coerce_to_regexp(pattern, true).match_from(self, 0)
      out = match.pre_match

      Regexp.last_match = match

      if replacement.equal?(undefined)
        replacement = yield(match[0].dup).to_s
        out.taint if replacement.tainted?
        out.append(replacement).append(match.post_match)
      else
        out.taint if replacement.tainted?
        StringValue(replacement).to_sub_replacement(out, match)
        out.append(match.post_match)
      end

      # We have to reset it again to match the specs
      Regexp.last_match = match

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

  def succ
    dup.succ!
  end

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

  def swapcase
    str = dup
    str.swapcase! || str
  end

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

  alias_method :intern, :to_sym

  def to_i(base=10)
    base = Rubinius::Type.coerce_to base, Integer, :to_int

    if base < 0 || base == 1 || base > 36
      raise ArgumentError, "illegal radix #{base}"
    end

    to_inum(base, false)
  end

  def to_s
    instance_of?(String) ? self : "".replace(self)
  end
  alias_method :to_str, :to_s

  def tr(source, replacement)
    str = dup
    str.tr!(source, replacement) || str
  end

  def tr!(source, replacement)
    tr_trans(source, replacement, false)
  end

  def tr_s(source, replacement)
    str = dup
    str.tr_s!(source, replacement) || str
  end

  def tr_s!(source, replacement)
    tr_trans(source, replacement, true)
  end

  def upcase
    str = dup
    str.upcase! || str
  end

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

  def tr_trans(source, replacement, squeeze)
    source = StringValue(source).dup
    replacement = StringValue(replacement).dup

    self.modify!

    return self.delete!(source) if replacement.empty?
    return if @num_bytes == 0

    invert = source[0] == ?^ && source.length > 1
    if invert
      source.slice!(0)
    end
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

      self.num_bytes = j if (j += 1) < @num_bytes
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

  def to_sub_replacement(result, match)
    index = 0
    while index < @num_bytes
      current = index
      while current < @num_bytes && @data[current] != 92  # ?\\
        current += 1
      end
      result.append(byteslice(index, current - index))
      break if current == @num_bytes

      # found backslash escape, looking next
      if current == @num_bytes - 1
        result.append("\\") # backslash at end of string
        break
      end
      index = current + 1

      cap = @data[index]

      additional = case cap
                when 38   # ?&
                  match[0]
                when 96   # ?`
                  match.pre_match
                when 39   # ?'
                  match.post_match
                when 43   # ?+
                  match.captures.compact[-1].to_s
                when 48..57   # ?0..?9
                  match[cap - 48].to_s
                when 92 # ?\\ escaped backslash
                  '\\'
                else     # unknown escape
                  '\\'.append(cap.chr)
                end
      result.append(additional)
      index += 1
    end
  end

  def to_inum(base, check)
    Rubinius.primitive :string_to_inum
    raise ArgumentError, "invalid value for Integer"
  end

  def apply_and!(other)
    Rubinius.primitive :string_apply_and
    raise PrimitiveFailure, "String#apply_and! primitive failed"
  end

  def compare_substring(other, start, size)
    Rubinius.primitive :string_compare_substring

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
      if str.size > 1 && str.getbyte(0) == 94 # ?^
        pos, neg = 0, 1
        str.slice!(0)
      else
        pos, neg = 1, 0
      end

      set = String.pattern 256, neg
      str.tr_expand! nil, true
      j, chars = -1, str.bytesize
      set.setbyte(str.getbyte(j), pos) while (j += 1) < chars

      table.apply_and! set
      i += 1
    end
    table
  end

  def tr_expand!(limit, invalid_as_empty)
    Rubinius.primitive :string_tr_expand
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

    str = self.class.pattern(1, "\0") * (padsize + @num_bytes)
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
    Rubinius.check_frozen

    if @shared
      @data = @data.dup
      @shared = nil
    end

    @ascii_only = @valid_encoding = nil
    @hash_value = nil # reset the hash value
  end

  def subpattern(pattern, capture)
    match = pattern.match(self)

    return nil unless match

    if index = Rubinius::Type.check_convert_type(capture, Fixnum, :to_int)
      return nil if index >= match.size || -index >= match.size
      capture = index
    end

    str = match[capture]
    str.taint if pattern.tainted?
    [match, str]
  end
  private :subpattern

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
      self.num_bytes -= count
    else
      # Resize if necessary
      new_size = @num_bytes - count + rsize
      # We use >= here and not > so we don't use every byte for
      # @data. We always want to have at least 1 character room
      # in a ByteArray for creating a \0 terminated string.
      resize_capacity new_size if new_size >= @data.size

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

        self.num_bytes += (rsize - count)
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
    self.num_bytes -= size
  end

  def dump
    s = self.class.allocate
    s.replace %{"#{transform(Rubinius::CType::Printed, false)}"}
    s
  end

  def shared!
    @shared = true
  end
end

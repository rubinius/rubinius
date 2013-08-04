# -*- encoding: us-ascii -*-

# Default Ruby Record Separator
# Used in this file and by various methods that need to ignore $/
DEFAULT_RECORD_SEPARATOR = "\n"

class String
  include Comparable

  def self.__allocate__
    Rubinius.primitive :string_allocate
    raise PrimitiveFailure, "String.allocate primitive failed"
  end

  def self.allocate
    str = __allocate__
    str.__data__ = Rubinius::ByteArray.allocate_sized(1)
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
    replace arg unless undefined.equal?(arg)
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
    Rubinius::Type.compatible_encoding self, other
    String.new(self) << other
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

    return false unless @num_bytes == other.bytesize
    return false unless Encoding.compatible?(self, other)
    return @data.compare_bytes(other.__data__, @num_bytes, other.bytesize) == 0
  end

  def =~(pattern)
    case pattern
    when Regexp
      match_data = pattern.search_region(self, 0, @num_bytes, true)
      Regexp.last_match = match_data
      return match_data.begin(0) if match_data
    when String
      raise TypeError, "type mismatch: String given"
    else
      pattern =~ self
    end
  end

  def [](index, other = undefined)
    Rubinius.primitive :string_aref

    unless undefined.equal?(other)
      if index.kind_of?(Fixnum) && other.kind_of?(Fixnum)
        return substring(index, other)
      elsif index.kind_of? Regexp
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
        Rubinius::Type.infect result, index
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

  def each_char
    return to_enum :each_char unless block_given?
    # TODO: Use Encodings for KCODE in 1.8 mode
    if Rubinius.kcode == :UTF8 and Rubinius.ruby18?
      scan(/./u) do |c|
        yield c
      end
    else
      i = 0
      n = size
      while i < n
        yield substring(i, 1)
        i += 1
      end

      self
    end
  end

  alias_method :chars, :each_char

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

    return false unless other.kind_of?(String) && other.bytesize == @num_bytes
    return false unless Encoding.compatible?(self, other)
    return @data.compare_bytes(other.__data__, @num_bytes, other.bytesize) == 0
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

  def insert(index, other)
    other = StringValue(other)
    index = Rubinius::Type.coerce_to index, Fixnum, :to_int
    index = length + 1 + index if index < 0

    if index > length or index < 0 then
      raise IndexError, "index #{index} out of string"
    end

    osize = other.bytesize
    size = @num_bytes + osize
    str = self.class.pattern size, "\0"

    self_m = Rubinius::Mirror.reflect self
    index = self_m.character_to_byte_index index

    Rubinius.check_frozen
    @hash_value = nil

    m = Rubinius::Mirror.reflect str
    if index == @num_bytes
      m.copy_from self, 0, @num_bytes, 0
      m.copy_from other, 0, osize, @num_bytes
    else
      m.copy_from self, 0, index, 0 if index > 0
      m.copy_from other, 0, osize, index
      m.copy_from self, index, @num_bytes - index, index + osize
    end

    self.num_bytes = size
    @data = str.__data__
    Rubinius::Type.infect self, other

    self
  end

  ControlCharacters = [10, 9, 7, 11, 12, 13, 27, 8]
  ControlPrintValue = ["\\n", "\\t", "\\a", "\\v", "\\f", "\\r", "\\e", "\\b"]

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
          index = fin + char.bytesize
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
                when 107 # \k named capture
                  if @data[index + 1] == 60
                    name = ""
                    i = index + 2
                    while i < @data.size && @data[i] != 62
                      name << @data[i]
                      i += 1
                    end
                    if i >= @data.size
                      '\\'.append(cap.chr)
                      index += 1
                      next
                    end
                    index = i
                    name.force_encoding result.encoding
                    match[name]
                  else
                    '\\'.append(cap.chr)
                  end
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

    i = 0
    size = strings.size
    while i < size
      str = StringValue(strings[i]).dup
      if str.bytesize > 1 && str.getbyte(0) == 94 # ?^
        pos = 0
        neg = 1
        str.slice!(0)
      else
        pos = 1
        neg = 0
      end

      set = String.pattern 256, neg
      set_data = set.__data__
      str.tr_expand! nil, true
      str_data = str.__data__
      j = -1
      chars = str.bytesize
      set_data[str_data[j]] = pos while (j += 1) < chars

      table.apply_and! set
      i += 1
    end
    table
  end

  def tr_expand!(limit, invalid_as_empty)
    Rubinius.primitive :string_tr_expand
    raise PrimitiveFailure, "String#tr_expand primitive failed"
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
    Rubinius::Type.infect str, pattern
    [match, str]
  end
  private :subpattern

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

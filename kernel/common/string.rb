# Default Ruby Record Separator
# Used in this file and by various methods that need to ignore $/
DEFAULT_RECORD_SEPARATOR = "\n"

class String
  include Enumerable
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

  alias_method :size, :size

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
    r = "#{self}#{StringValue(other)}"
    Rubinius::Type.infect(r, self)
    Rubinius::Type.infect(r, other)
    r
  end

  def <<(other)
    modify!

    unless other.kind_of? String
      if other.kind_of?(Integer) && other >= 0 && other <= 255
        other = other.chr
      else
        other = StringValue(other)
      end
    end

    Rubinius::Type.infect(self, other)
    append(other)
  end
  alias_method :concat, :<<

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
      return match_data.begin(0) if match_data
    when String
      raise TypeError, "type mismatch: String given"
    else
      pattern =~ self
    end
  end

  def []=(index, count_or_replacement, replacement=undefined)
    if undefined.equal?(replacement)
      replacement = count_or_replacement
      count = nil
    else
      count = count_or_replacement
    end

    case index
    when Fixnum
      index += @num_bytes if index < 0

      if count
        if index < 0 or index > @num_bytes
          raise IndexError, "index #{index} out of string"
        end

        count = Rubinius::Type.coerce_to count, Fixnum, :to_int

        if count < 0
          raise IndexError, "count is negative"
        end

        count = @num_bytes - index if index + count >= @num_bytes
      else
        if index < 0 or index >= @num_bytes
          raise IndexError, "index #{index} out of string"
        end
      end

      if not count and replacement.kind_of? Fixnum
        modify!
        @data[index] = replacement
      else
        replacement = StringValue replacement

        m = Rubinius::Mirror.reflect self
        m.splice index, count || 1, replacement
      end
    when String
      unless start = self.index(index)
        raise IndexError, "string not matched"
      end

      replacement = StringValue replacement

      m = Rubinius::Mirror.reflect self
      m.splice start, index.size, replacement
    when Range
      start = Rubinius::Type.coerce_to index.first, Fixnum, :to_int

      start += @num_bytes if start < 0

      if start < 0 or start > @num_bytes
        raise RangeError, "#{index.first} is out of range"
      end

      stop = Rubinius::Type.coerce_to index.last, Fixnum, :to_int
      stop += @num_bytes if stop < 0
      stop -= 1 if index.exclude_end?

      if stop < start
        bytes = 0
      elsif stop >= @num_bytes
        bytes = @num_bytes - start
      else
        bytes = stop + 1 - start
      end

      replacement = StringValue replacement

      m = Rubinius::Mirror.reflect self
      m.splice start, bytes, replacement
    when Regexp
      if count
        count = Rubinius::Type.coerce_to count, Fixnum, :to_int
      else
        count = 0
      end

      if match = index.match(self)
        ms = match.size
      else
        raise IndexError, "regexp does not match"
      end

      count += ms if count < 0 and -count < ms
      unless count < ms and count >= 0
        raise IndexError, "index #{count} out of match bounds"
      end

      unless match[count]
        raise IndexError, "regexp group #{count} not matched"
      end

      replacement = StringValue replacement

      m = Rubinius::Mirror.reflect self
      bi = m.byte_index match.begin(count)
      bs = m.byte_index(match.end(count)) - bi

      m.splice bi, bs, replacement
    else
      index = Rubinius::Type.coerce_to index, Fixnum, :to_int

      if count
        return self[index, count] = replacement
      else
        return self[index] = replacement
      end
    end

    Rubinius::Type.infect self, replacement

    return replacement
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

  def center(width, padstr = " ")
    justify width, :center, padstr
  end

  def ljust(width, padstr=" ")
    justify(width, :left, padstr)
  end

  def rjust(width, padstr = " ")
    justify(width, :right, padstr)
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
    m = Rubinius::Mirror.reflect str

    case direction
    when :right
      pad = String.pattern padsize, padstr
      m.copy_from pad, 0, padsize, 0
      m.copy_from self, 0, @num_bytes, padsize
    when :left
      pad = String.pattern padsize, padstr
      m.copy_from self, 0, @num_bytes, 0
      m.copy_from pad, 0, padsize, @num_bytes
    when :center
      half = padsize / 2.0
      lsize = half.floor
      rsize = half.ceil
      lpad = String.pattern lsize, padstr
      rpad = String.pattern rsize, padstr
      m.copy_from lpad, 0, lsize, 0
      m.copy_from self, 0, @num_bytes, lsize
      m.copy_from rpad, 0, rsize, lsize + @num_bytes
    end

    str
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

  def rindex(sub, finish=undefined)
    if undefined.equal?(finish)
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

  def insert(index, other)
    other = StringValue(other)
    index = Rubinius::Type.coerce_to index, Fixnum, :to_int
    index = length + 1 + index if index < 0

    if index > length or index < 0 then
      raise IndexError, "index #{index} out of string"
    end

    osize = other.size
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

  def <=>(other)
    if other.kind_of?(String)
      @data.compare_bytes(other.__data__, @num_bytes, other.size)
    else
      return unless other.respond_to?(:to_str) && other.respond_to?(:<=>)
      return unless tmp = (other <=> self)
      return -tmp # We're not supposed to convert to integer here
    end
  end

  def dump
    s = self.class.allocate
    s.replace %{"#{transform(Rubinius::CType::Printed, false)}"}
    s
  end

  def chomp!(sep=undefined)
    return if @num_bytes == 0

    if undefined.equal?(sep)
      sep = $/
    elsif sep
      sep = StringValue(sep)
    end

    return if sep.nil?

    if sep == DEFAULT_RECORD_SEPARATOR
      bytes = @num_bytes - 1
      case @data[bytes]
      when 13
        # do nothing
      when 10
        j = bytes - 1
        bytes = j if j >= 0 and @data[j] == 13
      else
        Rubinius.check_frozen
        return
      end
    elsif sep.size == 0
      bytes = @num_bytes
      i = bytes - 1

      while i >= 0
        break unless @data[i] == 10
        bytes = i
        j = i -= 1
        if j >= 0 and @data[j] == 13
          bytes = j
          i -= 1
        end
      end

      return if bytes == @num_bytes
    else
      size = sep.size
      return if size > @num_bytes

      # TODO: Move #compare_substring to mirror.
      return unless sep.compare_substring(self, -size, size) == 0
      bytes = @num_bytes - size
    end

    Rubinius.check_frozen

    # We do not need to dup the data, so don't use #modify!
    @hash_value = nil
    self.num_bytes = bytes

    self
  end

  def chomp(separator=$/)
    str = dup
    str.chomp!(separator) || str
  end

  def chop!
    return if @num_bytes == 0

    self.modify!

    if @num_bytes > 1 and
        @data[@num_bytes-1] == 10 and @data[@num_bytes-2] == 13
      self.num_bytes -= 2
    else
      self.num_bytes -= 1
    end

    self
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

        while @data[nxt] == 10 and nxt < @num_bytes
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

  alias_method :lines, :each_line
  alias_method :each, :each_line

  def each_char
    return to_enum :each_char unless block_given?

    if Rubinius.kcode == :UTF8
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

    return false unless other.kind_of?(String) && other.size == @num_bytes
    return false unless Encoding.compatible?(self, other)
    return @data.compare_bytes(other.__data__, @num_bytes, other.size) == 0
  end

  # This method is specifically part of 1.9 but we enable it in 1.8 also
  # because we need it internally.
  def getbyte(index)
    index = Rubinius::Type.coerce_to index, Fixnum, :to_int

    index += size if index < 0
    return if index < 0 or index >= size

    @data[index]
  end

  def gsub(pattern, replacement=undefined)
    # Because of the behavior of $~, this is duplicated from gsub! because
    # if we call gsub! from gsub, the last_match can't be updated properly.

    if undefined.equal? replacement
      unless block_given?
        return to_enum(:gsub, pattern, replacement)
      end

      tainted = false
    else
      tainted = replacement.tainted?
      unless replacement.kind_of? String
        replacement = StringValue(replacement)
        tainted ||= replacement.tainted?
      end
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true) unless pattern.kind_of? Regexp
    match = pattern.search_region(self, 0, @num_bytes, true)

    orig_len = @num_bytes
    orig_data = @data

    last_end = 0
    offset = nil

    last_match = nil

    ret = substring(0, 0) # Empty string and string subclass
    offset = match.full.at(0) if match

    while match
      if str = match.pre_match_from(last_end)
        ret.append str
      end

      if undefined.equal?(replacement)
        Regexp.last_match = match

        val = yield(match[0]).to_s
        tainted ||= val.tainted?
        ret.append val

        if !@data.equal?(orig_data) or @num_bytes != orig_len
          raise RuntimeError, "string modified"
        end
      else
        replacement.to_sub_replacement(ret, match)
      end

      tainted ||= val.tainted?

      last_end = match.full.at(1)

      if match.collapsing?
        if char = find_character(offset)
          offset += char.size
        else
          offset += 1
        end
      else
        offset = match.full.at(1)
      end

      last_match = match

      match = pattern.match_from self, offset
      break unless match

      offset = match.full.at(0)
    end

    Regexp.last_match = last_match

    str = substring(last_end, @num_bytes-last_end+1)
    ret.append str if str

    ret.taint if tainted || self.tainted?

    ret
  end

  def gsub!(pattern, replacement=undefined)
    # Because of the behavior of $~, this is duplicated from gsub! because
    # if we call gsub! from gsub, the last_match can't be updated properly.

    if undefined.equal? replacement
      unless block_given?
        return to_enum(:gsub, pattern, replacement)
      end

      tainted = false
    else
      tainted = replacement.tainted?
      unless replacement.kind_of? String
        replacement = StringValue(replacement)
        tainted ||= replacement.tainted?
      end
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true) unless pattern.kind_of? Regexp
    match = pattern.search_region(self, 0, @num_bytes, true)

    return nil unless match

    orig_len = @num_bytes
    orig_data = @data

    last_end = 0
    offset = nil

    last_match = nil

    ret = substring(0, 0) # Empty string and string subclass
    offset = match.full.at(0)

    while match
      if str = match.pre_match_from(last_end)
        ret.append str
      end

      if undefined.equal?(replacement)
        Regexp.last_match = match

        val = yield(match[0]).to_s
        tainted ||= val.tainted?
        ret.append val

        if !@data.equal?(orig_data) or @num_bytes != orig_len
          raise RuntimeError, "string modified"
        end
      else
        replacement.to_sub_replacement(ret, match)
      end

      tainted ||= val.tainted?

      last_end = match.full.at(1)

      if match.collapsing?
        if char = find_character(offset)
          offset += char.size
        else
          offset += 1
        end
      else
        offset = match.full.at(1)
      end

      last_match = match

      match = pattern.match_from self, offset
      break unless match

      offset = match.full.at(0)
    end

    Regexp.last_match = last_match

    str = substring(last_end, @num_bytes-last_end+1)
    ret.append str if str

    ret.taint if tainted || self.tainted?

    replace(ret)
    self
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

  def inspect
    "\"#{transform(Rubinius::CType::Printed, true)}\""
  end

  ControlCharacters = [10, 9, 7, 11, 12, 13, 27, 8]
  ControlPrintValue = ["\\n", "\\t", "\\a", "\\v", "\\f", "\\r", "\\e", "\\b"]

  def lstrip!
    return if @num_bytes == 0

    start = 0

    ctype = Rubinius::CType

    while start < @num_bytes && ctype.isspace(@data[start])
      start += 1
    end

    return if start == 0

    modify!
    self.num_bytes -= start
    @data.move_bytes start, @num_bytes, 0
    self
  end

  def lstrip
    str = dup
    str.lstrip! || str
  end

  def oct
    to_inum(-8, false)
  end

  def replace(other)
    # If we're replacing with ourselves, then we have nothing to do
    return self if equal?(other)

    Rubinius.check_frozen

    other = StringValue(other)

    @shared = true
    other.shared!
    @data = other.__data__
    self.num_bytes = other.num_bytes
    @hash_value = nil

    Rubinius::Type.infect(self, other)
  end

  def initialize_copy(other)
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
    self.num_bytes = stop
    self
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

    index += size if index < 0
    if index < 0 or index >= size
      raise IndexError, "byte index #{index} is outside bounds of String"
    end

    @ascii_only = @valid_encoding = nil
    @data[index] = byte
  end

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

  def unpack(directives)
    Rubinius.primitive :string_unpack

    unless directives.kind_of? String
      return unpack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
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

  def to_sub_replacement(result, match)
    index = 0
    while index < @num_bytes
      current = index
      while current < @num_bytes && @data[current] != 92  # ?\\
        current += 1
      end
      result.append(substring(index, current - index))
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
      if str.size > 1 && str.getbyte(0) == 94 # ?^
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
      chars = str.size
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

  def match(pattern)
    pattern = Rubinius::Type.coerce_to_regexp(pattern) unless pattern.kind_of? Regexp
    match_data = pattern.search_region(self, 0, @num_bytes, true)
    Regexp.last_match = match_data
    match_data
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

  def slice!(one, two=undefined)
    # This is un-DRY, but it's a simple manual argument splitting. Keeps
    # the code fast and clean since the sequence are pretty short.
    #
    if undefined.equal?(two)
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

  def sub(pattern, replacement=undefined)
    if undefined.equal?(replacement) and !block_given?
      raise ArgumentError, "wrong number of arguments (1 for 2)"
    end

    unless pattern
      raise ArgumentError, "wrong number of arguments (0 for 2)"
    end

    if match = Rubinius::Type.coerce_to_regexp(pattern, true).match_from(self, 0)
      out = match.pre_match

      Regexp.last_match = match

      if undefined.equal?(replacement)
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

  def sub!(pattern, replacement=undefined)
    # Copied mostly from sub to keep Regexp.last_match= working right.

    if undefined.equal?(replacement) and !block_given?
      raise ArgumentError, "wrong number of arguments (1 for 2)"
    end

    unless pattern
      raise ArgumentError, "wrong number of arguments (0 for 2)"
    end

    if match = Rubinius::Type.coerce_to_regexp(pattern, true).match_from(self, 0)
      out = match.pre_match

      Regexp.last_match = match

      if undefined.equal?(replacement)
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
      m = Rubinius::Mirror.reflect self
      m.splice last_alnum, 1, carry.chr + @data[last_alnum].chr
    end

    return self
  end

  alias_method :next, :succ
  alias_method :next!, :succ!

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

  def shared!
    @shared = true
  end
end

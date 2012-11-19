# -*- encoding: us-ascii -*-

class String
  include Enumerable

  alias_method :bytesize, :size

  def inspect
    "\"#{transform(Rubinius::CType::Printed, true)}\""
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

  def reverse!
    return self if @num_bytes <= 1
    self.modify!

    @data.reverse(0, @num_bytes)
    self
  end

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

  def sub!(pattern, replacement=undefined)
    # Copied mostly from sub to keep Regexp.last_match= working right.

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
      splice! last_alnum, 1, carry.chr + @data[last_alnum].chr
    end

    return self
  end

  alias_method :next, :succ
  alias_method :next!, :succ!

  def unpack(directives)
    Rubinius.primitive :string_unpack18

    unless directives.kind_of? String
      return unpack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
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

  # NOTE: TypeError is raised in String#replace and not in String#chomp! when
  # self is frozen. This is intended behaviour.
  def chomp!(sep=undefined)
    # special case for performance. No seperator is by far the most common usage.
    if sep.equal?(undefined)
      return if @num_bytes == 0

      Rubinius.check_frozen

      c = @data[@num_bytes-1]
      if c == 10 # ?\n
        self.num_bytes -= 1 if @num_bytes > 1 && @data[@num_bytes-2] == 13 # ?\r
      elsif c != 13 # ?\r
        return
      end

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      self.num_bytes -= 1
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

      Rubinius.check_frozen

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      self.num_bytes -= 1
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

      Rubinius.check_frozen

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      self.num_bytes = size
    else
      size = sep.size
      return if size > @num_bytes || sep.compare_substring(self, -size, size) != 0

      Rubinius.check_frozen

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      self.num_bytes -= size
    end

    return self
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
  alias_method :initialize_copy, :replace
  # private :initialize_copy

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

  def lines(sep=$/)
    return to_enum(:lines, sep) unless block_given?

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

        str = byteslice(pos, match_size)
        yield str unless str.empty?

        # detect mutation within the block
        if !@data.equal?(orig_data) or @num_bytes != size
          raise RuntimeError, "string modified while iterating"
        end

        pos = nxt
      end

      # No more separates, but we need to grab the last part still.
      fin = byteslice(pos, @num_bytes - pos)
      yield fin if fin and !fin.empty?

    else

      # This is the normal case.
      pat_size = sep.size

      while pos < size
        nxt = find_string(sep, pos)
        break unless nxt

        match_size = nxt - pos
        str = byteslice(pos, match_size + pat_size)
        yield str unless str.empty?

        # detect mutation within the block
        if !@data.equal?(orig_data) or @num_bytes != size
          raise RuntimeError, "string modified while iterating"
        end

        pos = nxt + pat_size
      end

      # No more separates, but we need to grab the last part still.
      fin = byteslice(pos, @num_bytes - pos)
      yield fin unless fin.empty?
    end

    self
  end

  alias_method :each_line, :lines
  alias_method :each, :lines

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

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true)
    orig_len = @num_bytes
    orig_data = @data

    last_end = 0
    offset = nil
    ret = byteslice(0, 0) # Empty string and string subclass

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
        ret.append byteslice(last_end, pre_len)
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
        replacement.to_sub_replacement(ret, match)
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

    str = byteslice(last_end, @num_bytes-last_end+1)
    ret.append str if str

    ret.taint if tainted || self.tainted?
    return ret
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

    last_end = 0
    offset = nil

    last_match = nil

    ret = byteslice(0, 0) # Empty string and string subclass
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
        replacement.to_sub_replacement(ret, match)
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

    str = byteslice(last_end, @num_bytes-last_end+1)
    ret.append str if str

    ret.taint if tainted || self.tainted?

    if last_match
      replace(ret)
      return self
    else
      return nil
    end
  end

  def match(pattern)
    match_data = Rubinius::Type.coerce_to_regexp(pattern).search_region(self, 0, @num_bytes, true)
    Regexp.last_match = match_data
    return match_data
  end

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
end

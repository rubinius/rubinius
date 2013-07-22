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

  alias_method :lines, :each_line
  alias_method :each, :each_line

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

    ret = byteslice(0, 0) # Empty string and string subclass
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
          offset += char.bytesize
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

    str = byteslice(last_end, @num_bytes-last_end+1)
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

    ret = byteslice(0, 0) # Empty string and string subclass
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
          offset += char.bytesize
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

    str = byteslice(last_end, @num_bytes-last_end+1)
    ret.append str if str

    ret.taint if tainted || self.tainted?

    replace(ret)
    self
  end

  def match(pattern)
    pattern = Rubinius::Type.coerce_to_regexp(pattern) unless pattern.kind_of? Regexp
    match_data = pattern.search_region(self, 0, @num_bytes, true)
    Regexp.last_match = match_data
    match_data
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
      m.splice start, index.bytesize, replacement
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
      @data.compare_bytes(other.__data__, @num_bytes, other.bytesize)
    else
      return unless other.respond_to?(:to_str) && other.respond_to?(:<=>)
      return unless tmp = (other <=> self)
      return -tmp # We're not supposed to convert to integer here
    end
  end
end

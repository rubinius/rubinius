# -*- encoding: us-ascii -*-

class String
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, String, :to_str
  end

  def codepoints
    return to_enum :codepoints unless block_given?

    chars { |c| yield c.ord }
    self
  end

  alias_method :each_codepoint, :codepoints

  def encode!(to=undefined, from=undefined, options=undefined)
    Rubinius.check_frozen

    replace encode(to, from, options)
  end

  def encode(to=undefined, from=undefined, options=undefined)
    # TODO
    if to.equal? undefined
      to = Encoding.default_internal
      return self.dup unless to
    else
      to = Rubinius::Type.coerce_to_encoding to
    end

    if from.equal? undefined
      from = encoding
      options = 0
    end

    if options.equal? undefined
      if from.kind_of? Hash
        options = from
        from = encoding
      else
        from = Rubinius::Type.coerce_to_encoding from
        options = 0
      end
    end

    if from == to
      str = self.dup
    else
      ec = Encoding::Converter.new from, to, options
      str = ec.convert self
    end

    # TODO: replace this hack with transcoders
    if options.kind_of? Hash
      case xml = options[:xml]
      when :text
        str.gsub!(/[&><]/, '&' => '&amp;', '>' => '&gt;', '<' => '&lt;')
      when :attr
        str.gsub!(/[&><"]/, '&' => '&amp;', '>' => '&gt;', '<' => '&lt;', '"' => '&quot;')
        str.insert(0, '"')
        str.insert(-1, '"')
      when nil
        # nothing
      else
        raise ArgumentError, "unexpected value for xml option: #{xml.inspect}"
      end
    end

    str
  end

  def force_encoding(enc)
    @ascii_only = @valid_encoding = nil
    @encoding = Rubinius::Type.coerce_to_encoding enc
    self
  end

  def inspect
    result_encoding = Encoding.default_internal || Encoding.default_external
    unless result_encoding.ascii_compatible?
      result_encoding = Encoding::US_ASCII
    end

    enc = encoding
    ascii = enc.ascii_compatible?
    enc_name = enc.name
    unicode = enc_name.start_with?("UTF-") && enc_name[4] != ?7

    if unicode
      if enc.equal? Encoding::UTF_16
        a = getbyte 0
        b = getbyte 1

        if a == 0xfe and b == 0xff
          enc = Encoding::UTF_16BE
        elsif a == 0xff and b == 0xfe
          enc = Encoding::UTF_16LE
        else
          unicode = false
        end
      elsif enc.equal? Encoding::UTF_32
        a = getbyte 0
        b = getbyte 1
        c = getbyte 2
        d = getbyte 3

        if a == 0 and b == 0 and c == 0xfe and d == 0xfe
          enc = Encoding::UTF_32BE
        elsif a == 0xff and b == 0xfe and c == 0 and d == 0
          enc = Encoding::UTF_32LE
        else
          unicode = false
        end
      end
    end

    array = []

    index = 0
    total = bytesize
    while index < total
      char = chr_at index

      if char
        bs = char.bytesize

        if (ascii or unicode) and bs == 1
          escaped = nil

          byte = getbyte(index)
          if byte >= 7 and byte <= 92
            case byte
            when 7  # \a
              escaped = '\a'
            when 8  # \b
              escaped = '\b'
            when 9  # \t
              escaped = '\t'
            when 10 # \n
              escaped = '\n'
            when 11 # \v
              escaped = '\v'
            when 12 # \f
              escaped = '\f'
            when 13 # \r
              escaped = '\r'
            when 27 # \e
              escaped = '\e'
            when 34 # \"
              escaped = '\"'
            when 35 # #
              case getbyte(index += 1)
              when 36   # $
                escaped = '\#$'
              when 64   # @
                escaped = '\#@'
              when 123  # {
                escaped = '\#{'
              else
                index -= 1
              end
            when 92 # \\
              escaped = '\\\\'
            end

            if escaped
              array << escaped
              index += 1
              next
            end
          end
        end

        if char.printable?
          array << char
        else
          code = char.ord
          escaped = code.to_s(16).upcase

          if unicode
            if code < 0x10000
              pad = "0" * (4 - escaped.bytesize)
              array << "\\u#{pad}#{escaped}"
            else
              array << "\\u{#{escaped}}"
            end
          else
            if code < 0x100
              pad = "0" * (2 - escaped.bytesize)
              array << "\\x#{pad}#{escaped}"
            else
              array << "\\x{#{escaped}}"
            end
          end
        end

        index += bs
      else
        array << "\\x#{getbyte(index).to_s(16)}"
        index += 1
      end
    end

    size = array.inject(0) { |s, chr| s += chr.bytesize }
    result = String.pattern size + 2, ?".ord

    index = 1
    array.each do |chr|
      result.copy_from chr, 0, chr.bytesize, index
      index += chr.bytesize
    end

    Rubinius::Type.infect result, self
    result.force_encoding result_encoding
  end

  def prepend(other)
    self[0,0] = other
    self
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
      unless stop.size < size
        after_stop = exclusive ? stop : stop.succ
        current = self

        until current == after_stop
          yield current
          current = StringValue(current.succ)
          break if current.size > stop.size || current.size == 0
        end
      end
    end
    self
  end

  def reverse!
    Rubinius.check_frozen

    return self if @num_bytes <= 1
    self.modify!

    @data.reverse(0, @num_bytes)
    self
  end

  def sub!(pattern, replacement=undefined)
    # Copied mostly from sub to keep Regexp.last_match= working right.

    if replacement.equal?(undefined) and !block_given?
      raise ArgumentError, "wrong number of arguments (1 for 2)"
    end

    unless pattern
      raise ArgumentError, "wrong number of arguments (0 for 2)"
    end

    Rubinius.check_frozen

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

  def to_c
    Complexifier.new(self).convert
  end

  def to_r
    Rationalizer.new(self).convert
  end

  def unpack(directives)
    Rubinius.primitive :string_unpack19

    unless directives.kind_of? String
      return unpack(StringValue(directives))
    end

    raise ArgumentError, "invalid directives string: #{directives}"
  end

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
    self.num_bytes = stop
    self
  end

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
    self.num_bytes -= start
    @data.move_bytes start, @num_bytes, 0
    self
  end

  def chop!
    Rubinius.check_frozen
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
    Rubinius.check_frozen

    # special case for performance. No seperator is by far the most common usage.
    if sep.equal?(undefined)
      return if @num_bytes == 0

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
        self.num_bytes -= 1 if @num_bytes > 1 && @data[@num_bytes-2] == 13 # ?\r
      elsif c != 13 # ?\r
        return
      end

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

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      self.num_bytes = size
    else
      size = sep.size
      return if size > @num_bytes || sep.compare_substring(self, -size, size) != 0

      # don't use modify! because it will dup the data when we don't need to.
      @hash_value = nil
      self.num_bytes -= size
    end

    return self
  end

  def clear
    Rubinius.check_frozen
    self.num_bytes = 0
    self
  end

  def replace(other)
    Rubinius.check_frozen

    # If we're replacing with ourselves, then we have nothing to do
    return self if equal?(other)

    other = StringValue(other)

    @shared = true
    other.shared!
    @data = other.__data__
    self.num_bytes = other.num_bytes
    @hash_value = nil
    force_encoding(other.encoding)

    Rubinius::Type.infect(self, other)
  end
  alias_method :initialize_copy, :replace
  # private :initialize_copy

  def <<(other)
    modify!

    if other.kind_of? Integer
      if encoding == Encoding::US_ASCII and other >= 128 and other < 256
        force_encoding(Encoding::ASCII_8BIT)
      end

      other = other.chr(encoding)
    end
    unless other.kind_of? String
      other = StringValue(other)
    end

    unless other.encoding == encoding
      enc = Rubinius::Type.compatible_encoding self, other
      force_encoding enc
    end

    Rubinius::Type.infect(self, other)
    append(other)
  end
  alias_method :concat, :<<

  def chr
    substring 0, 1
  end

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

        str = byteslice pos, match_size
        yield str unless str.empty?

        # detect mutation within the block
        if !@data.equal?(orig_data) or @num_bytes != size
          raise RuntimeError, "string modified while iterating"
        end

        pos = nxt
      end

      # No more separates, but we need to grab the last part still.
      fin = byteslice pos, @num_bytes - pos
      yield fin if fin and !fin.empty?

    else

      # This is the normal case.
      pat_size = sep.size
      unmodified_self = clone

      while pos < size
        nxt = unmodified_self.find_string(sep, pos)
        break unless nxt

        match_size = nxt - pos
        str = unmodified_self.byteslice pos, match_size + pat_size
        yield str unless str.empty?

        pos = nxt + pat_size
      end

      # No more separates, but we need to grab the last part still.
      fin = unmodified_self.byteslice pos, @num_bytes - pos
      yield fin unless fin.empty?
    end

    self
  end

  alias_method :each_line, :lines

  def gsub(pattern, replacement=undefined)
    unless block_given? or replacement != undefined
      return to_enum(:gsub, pattern, replacement)
    end

    tainted = false
    untrusted = untrusted?

    if replacement.equal?(undefined)
      use_yield = true
    else
      tainted = replacement.tainted?
      untrusted ||= replacement.untrusted?
      hash = Rubinius::Type.check_convert_type(replacement, Hash, :to_hash)
      replacement = StringValue(replacement) unless hash
      tainted ||= replacement.tainted?
      untrusted ||= replacement.untrusted?
      use_yield = false
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true)
    orig_len = @num_bytes
    orig_data = @data

    last_end = 0
    offset = nil
    ret = byteslice 0, 0 # Empty string and string subclass

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

      if use_yield || hash
        Regexp.last_match = match

        if use_yield
          val = yield match.to_s
        else
          val = hash[match.to_s]
        end
        untrusted = true if val.untrusted?
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
          offset += char.bytesize
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

    str = byteslice last_end, @num_bytes-last_end+1
    ret.append str if str

    ret.taint if tainted || self.tainted?
    ret.untrust if untrusted
    return ret
  end

  def gsub!(pattern, replacement=undefined)
    unless block_given? or replacement != undefined
      return to_enum(:gsub, pattern, replacement)
    end

    Rubinius.check_frozen

    tainted = false
    untrusted = untrusted?

    if replacement.equal?(undefined)
      use_yield = true
    else
      tainted = replacement.tainted?
      untrusted ||= replacement.untrusted?
      hash = Rubinius::Type.check_convert_type(replacement, Hash, :to_hash)
      replacement = StringValue(replacement) unless hash
      tainted ||= replacement.tainted?
      untrusted ||= replacement.untrusted?
      use_yield = false
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true)
    orig_len = @num_bytes
    orig_data = @data

    last_end = 0
    offset = nil
    ret = byteslice 0, 0 # Empty string and string subclass

    last_match = nil
    match = pattern.match_from self, last_end

    if match
      ma_range = match.full
      ma_start = ma_range.at(0)
      ma_end   = ma_range.at(1)

      offset = ma_start
    else
      Regexp.last_match = nil
      return nil
    end

    while match
      nd = ma_start - 1
      pre_len = nd-last_end+1

      if pre_len > 0
        ret.append byteslice(last_end, pre_len)
      end

      if use_yield || hash
        Regexp.last_match = match

        if use_yield
          val = yield match.to_s
        else
          val = hash[match.to_s]
        end
        untrusted = true if val.untrusted?
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
          offset += char.bytesize
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

    str = byteslice last_end, @num_bytes-last_end+1
    ret.append str if str

    self.taint if tainted
    self.untrust if untrusted

    replace(ret)
    return self
  end

  def match(pattern, pos=0)
    match_data = Rubinius::Type.coerce_to_regexp(pattern).search_region(self, pos, @num_bytes, true)
    Regexp.last_match = match_data
    if match_data && block_given?
      yield match_data
    else
      match_data
    end
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
        raise IndexError, "index #{index} out of string" if index > @num_bytes
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

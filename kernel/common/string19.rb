# -*- encoding: us-ascii -*-

class String
  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, String, :to_str
  end

  def each_codepoint
    return to_enum :each_codepoint unless block_given?

    each_char { |c| yield c.ord }
    self
  end

  alias_method :codepoints, :each_codepoint

  def encode!(to=undefined, from=undefined, options=undefined)
    Rubinius.check_frozen

    case to
    when Encoding
      to_enc = to
    when Hash
      options = to
      to_enc = Encoding.default_internal
    when undefined
      to_enc = Encoding.default_internal
      return self unless to_enc
    else
      opts = Rubinius::Type::check_convert_type to, Hash, :to_hash

      if opts
        options = opts
        to_enc = Encoding.default_internal
      else
        to_enc = Rubinius::Type.try_convert_to_encoding to
      end
    end

    case from
    when undefined
      from_enc = encoding
    when Encoding
      from_enc = from
    when Hash
      options = from
      from_enc = encoding
    else
      opts = Rubinius::Type::check_convert_type from, Hash, :to_hash

      if opts
        options = opts
        from_enc = encoding
      else
        from_enc = Rubinius::Type.coerce_to_encoding from
      end
    end

    if from_enc.equal? undefined or to_enc.equal? undefined
      raise Encoding::ConverterNotFoundError, "undefined code converter (#{from} to #{to})"
    end

    case options
    when undefined
      options = 0
    when Hash
      # do nothing
    else
      options = Rubinius::Type.coerce_to options, Hash, :to_hash
    end

    if ascii_only? and to_enc.ascii_compatible?
      force_encoding to_enc
    elsif to_enc and from_enc != to_enc
      ec = Encoding::Converter.new from_enc, to_enc, options
      dest = ""
      status = ec.primitive_convert self, dest, nil, nil, ec.options
      raise ec.last_error unless status == :finished
      replace dest
    end

    # TODO: replace this hack with transcoders
    if options.kind_of? Hash
      case xml = options[:xml]
      when :text
        gsub!(/[&><]/, '&' => '&amp;', '>' => '&gt;', '<' => '&lt;')
      when :attr
        gsub!(/[&><"]/, '&' => '&amp;', '>' => '&gt;', '<' => '&lt;', '"' => '&quot;')
        insert(0, '"')
        insert(-1, '"')
      when nil
        # nothing
      else
        raise ArgumentError, "unexpected value for xml option: #{xml.inspect}"
      end
    end

    self
  end

  def encode(to=undefined, from=undefined, options=undefined)
    dup.encode! to, from, options
  end

  def force_encoding(enc)
    @encoding = Rubinius::Type.coerce_to_encoding enc
    unless @ascii_only && @encoding.ascii_compatible?
      @ascii_only = @valid_encoding = @num_chars = nil
    end
    if bytesize == 0 && @encoding.ascii_compatible?
      @ascii_only = true
      @valid_encoding = true
      @num_chars = 0
    end
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
    m = Rubinius::Mirror.reflect result

    index = 1
    array.each do |chr|
      m.copy_from chr, 0, chr.bytesize, index
      index += chr.bytesize
    end

    Rubinius::Type.infect result, self
    result.force_encoding result_encoding
  end

  def prepend(other)
    self[0, 0] = other
    self
  end

  def upto(stop, exclusive=false)
    return to_enum :upto, stop, exclusive unless block_given?
    stop = StringValue(stop)

    if stop.size == 1 && size == 1
      return self if self > stop
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

  def sub(pattern, replacement=undefined)
    # Because of the behavior of $~, this is duplicated from sub! because
    # if we call sub! from sub, the last_match can't be updated properly.

    unless valid_encoding?
      raise ArgumentError, "invalid byte sequence in #{encoding}"
    end

    if undefined.equal? replacement
      unless block_given?
        return to_enum(:sub, pattern, replacement)
      end
      use_yield = true
      tainted = false
    else
      tainted = replacement.tainted?
      untrusted = replacement.untrusted?

      unless replacement.kind_of?(String)
        hash = Rubinius::Type.check_convert_type(replacement, Hash, :to_hash)
        replacement = StringValue(replacement) unless hash
        tainted ||= replacement.tainted?
        untrusted ||= replacement.untrusted?
      end
      use_yield = false
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true) unless pattern.kind_of? Regexp
    match = pattern.match_from(self, 0)

    Regexp.last_match = match

    ret = byteslice(0, 0) # Empty string and string subclass

    if match
      ret.append match.pre_match

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
      else
        replacement.to_sub_replacement(ret, match)
      end

      ret.append(match.post_match)
      tainted ||= val.tainted?
    else
      return self
    end

    ret.taint if tainted
    ret.untrust if untrusted

    ret
  end

  def sub!(pattern, replacement=undefined)
    # Because of the behavior of $~, this is duplicated from sub! because
    # if we call sub! from sub, the last_match can't be updated properly.

    unless valid_encoding?
      raise ArgumentError, "invalid byte sequence in #{encoding}"
    end

    if undefined.equal? replacement
      unless block_given?
        return to_enum(:sub, pattern, replacement)
      end
      Rubinius.check_frozen
      use_yield = true
      tainted = false
    else
      Rubinius.check_frozen
      tainted = replacement.tainted?
      untrusted = replacement.untrusted?

      unless replacement.kind_of?(String)
        hash = Rubinius::Type.check_convert_type(replacement, Hash, :to_hash)
        replacement = StringValue(replacement) unless hash
        tainted ||= replacement.tainted?
        untrusted ||= replacement.untrusted?
      end
      use_yield = false
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true) unless pattern.kind_of? Regexp
    match = pattern.match_from(self, 0)

    Regexp.last_match = match

    ret = byteslice(0, 0) # Empty string and string subclass

    if match
      ret.append match.pre_match

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
      else
        replacement.to_sub_replacement(ret, match)
      end

      ret.append(match.post_match)
      tainted ||= val.tainted?
    else
      return nil
    end

    ret.taint if tainted
    ret.untrust if untrusted

    replace(ret)
    self
  end

  def slice!(one, two=undefined)
    Rubinius.check_frozen
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
      m = Rubinius::Mirror.reflect self
      m.splice last_alnum, 1, carry.chr + @data[last_alnum].chr
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

    m = Rubinius::Mirror.reflect self

    bytes = m.previous_byte_index @num_bytes
    return unless bytes

    chr = chr_at bytes
    if chr.ord == 10 and chr.ascii?
      if i = m.previous_byte_index(bytes)
        chr = chr_at i

        bytes = i if chr.ord == 13 and chr.ascii?
      end
    end

    self.num_bytes = bytes

    # We do not need to dup the data, so don't use #modify!
    @hash_value = nil

    self
  end

  def chomp!(sep=undefined)
    Rubinius.check_frozen

    if undefined.equal?(sep)
      sep = $/
    elsif sep
      sep = StringValue(sep)
    end

    return if sep.nil?

    m = Rubinius::Mirror.reflect self

    if sep == DEFAULT_RECORD_SEPARATOR
      return unless bytes = m.previous_byte_index(@num_bytes)

      chr = chr_at bytes
      return unless chr.ascii?

      code = chr.ord
      case chr.ord
      when 13
        # do nothing
      when 10
        if j = m.previous_byte_index(bytes)
          chr = chr_at j

          if chr.ord == 13 and chr.ascii?
            bytes = j
          end
        end
      else
        return
      end
    elsif sep.size == 0
      return if @num_bytes == 0
      bytes = @num_bytes

      while i = m.previous_byte_index(bytes)
        chr = chr_at i
        break unless chr.ord == 10 and chr.ascii?

        bytes = i

        if j = m.previous_byte_index(i)
          chr = chr_at j
          if chr.ord == 13 and chr.ascii?
            bytes = j
          end
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

    # We do not need to dup the data, so don't use #modify!
    @hash_value = nil
    self.num_bytes = bytes

    self
  end

  def clear
    Rubinius.check_frozen
    self.num_bytes = 0
    self
  end

  def replace(other)
    Rubinius.check_frozen

    # If we're replacing with ourselves, then we have nothing to do
    return self if Rubinius::Type.object_equal(self, other)

    other = StringValue(other)

    @shared = true
    other.shared!
    @data = other.__data__
    self.num_bytes = other.num_bytes
    @hash_value = nil
    force_encoding(other.encoding)

    Rubinius::Type.infect(self, other)
  end

  def initialize_copy(other)
  end

  def <<(other)
    Rubinius.check_frozen

    unless other.kind_of? String
      if other.kind_of? Integer
        if encoding == Encoding::US_ASCII and other >= 128 and other < 256
          force_encoding(Encoding::ASCII_8BIT)
        end

        other = other.chr(encoding)
      else
        other = StringValue(other)
      end
    end

    Rubinius::Type.infect(self, other)
    append(other)
  end
  alias_method :concat, :<<

  def chr
    substring 0, 1
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

  alias_method :lines, :each_line

  def gsub(pattern, replacement=undefined)
    # Because of the behavior of $~, this is duplicated from gsub! because
    # if we call gsub! from gsub, the last_match can't be updated properly.

    unless valid_encoding?
      raise ArgumentError, "invalid byte sequence in #{encoding}"
    end

    if undefined.equal? replacement
      unless block_given?
        return to_enum(:gsub, pattern, replacement)
      end
      use_yield = true
      tainted = false
    else
      tainted = replacement.tainted?
      untrusted = replacement.untrusted?

      unless replacement.kind_of?(String)
        hash = Rubinius::Type.check_convert_type(replacement, Hash, :to_hash)
        replacement = StringValue(replacement) unless hash
        tainted ||= replacement.tainted?
        untrusted ||= replacement.untrusted?
      end
      use_yield = false
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true) unless pattern.kind_of? Regexp
    match = pattern.search_region(self, 0, @num_bytes, true)

    unless match
      Regexp.last_match = nil
    end

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
    if str
      ret.append str
    end

    ret.taint if tainted
    ret.untrust if untrusted

    ret
  end

  def gsub!(pattern, replacement=undefined)
    # Because of the behavior of $~, this is duplicated from gsub! because
    # if we call gsub! from gsub, the last_match can't be updated properly.

    unless valid_encoding?
      raise ArgumentError, "invalid byte sequence in #{encoding}"
    end

    if undefined.equal? replacement
      unless block_given?
        return to_enum(:gsub, pattern, replacement)
      end
      Rubinius.check_frozen
      use_yield = true
      tainted = false
    else
      Rubinius.check_frozen
      tainted = replacement.tainted?
      untrusted = replacement.untrusted?

      unless replacement.kind_of?(String)
        hash = Rubinius::Type.check_convert_type(replacement, Hash, :to_hash)
        replacement = StringValue(replacement) unless hash
        tainted ||= replacement.tainted?
        untrusted ||= replacement.untrusted?
      end
      use_yield = false
    end

    pattern = Rubinius::Type.coerce_to_regexp(pattern, true) unless pattern.kind_of? Regexp
    match = pattern.search_region(self, 0, @num_bytes, true)

    unless match
      Regexp.last_match = nil
      return nil
    end

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
    if str
      ret.append str
    end

    ret.taint if tainted
    ret.untrust if untrusted

    replace(ret)
    self
  end

  def match(pattern, pos=0)
    pattern = Rubinius::Type.coerce_to_regexp(pattern) unless pattern.kind_of? Regexp

    m = Rubinius::Mirror.reflect self
    pos = pos < 0 ? pos + size : pos
    pos = m.character_to_byte_index pos
    match_data = pattern.search_region(self, pos, @num_bytes, true)
    Regexp.last_match = match_data
    if match_data && block_given?
      yield match_data
    else
      match_data
    end
  end

  def []=(index, count_or_replacement, replacement=undefined)
    if undefined.equal?(replacement)
      replacement = count_or_replacement
      count = nil
    else
      count = count_or_replacement
    end

    m = Rubinius::Mirror.reflect self

    case index
    when Fixnum
      index += size if index < 0

      if index < 0 or index > size
        raise IndexError, "index #{index} out of string"
      end

      unless bi = m.byte_index(index)
        raise IndexError, "unable to find character at: #{index}"
      end

      if count
        count = Rubinius::Type.coerce_to count, Fixnum, :to_int

        if count < 0
          raise IndexError, "count is negative"
        end

        total = index + count
        if total >= size
          bs = bytesize - bi
        else
          bs = m.byte_index(total) - bi
        end
      else
        bs = index == size ? 0 : m.byte_index(index + 1) - bi
      end

      replacement = StringValue replacement
      enc = Rubinius::Type.compatible_encoding self, replacement

      m.splice bi, bs, replacement
    when String
      unless start = m.byte_index(index)
        raise IndexError, "string not matched"
      end

      replacement = StringValue replacement
      enc = Rubinius::Type.compatible_encoding self, replacement

      m.splice start, index.bytesize, replacement
    when Range
      start = Rubinius::Type.coerce_to index.first, Fixnum, :to_int

      start += size if start < 0

      if start < 0 or start > size
        raise RangeError, "#{index.first} is out of range"
      end

      unless bi = m.byte_index(start)
        raise IndexError, "unable to find character at: #{start}"
      end

      stop = Rubinius::Type.coerce_to index.last, Fixnum, :to_int
      stop += size if stop < 0
      stop -= 1 if index.exclude_end?

      if stop < start
        bs = 0
      elsif stop >= size
        bs = bytesize - bi
      else
        bs = m.byte_index(stop + 1) - bi
      end

      replacement = StringValue replacement
      enc = Rubinius::Type.compatible_encoding self, replacement

      m.splice bi, bs, replacement
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
      enc = Rubinius::Type.compatible_encoding self, replacement

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
    force_encoding enc

    return replacement
  end

  def center(width, padding=" ")
    padding = StringValue(padding)
    raise ArgumentError, "zero width padding" if padding.size == 0

    enc = Rubinius::Type.compatible_encoding self, padding

    width = Rubinius::Type.coerce_to width, Fixnum, :to_int
    return dup if width <= size

    width -= size
    left = width / 2

    bs = bytesize
    pbs = padding.bytesize

    if pbs > 1
      ps = padding.size
      pm = Rubinius::Mirror.reflect padding

      x = left / ps
      y = left % ps

      lpbi = pm.byte_index(y)
      lbytes = x * pbs + lpbi

      right = left + (width & 0x1)

      x = right / ps
      y = right % ps

      rpbi = pm.byte_index(y)
      rbytes = x * pbs + rpbi

      pad = self.class.pattern rbytes, padding
      str = self.class.pattern lbytes + bs + rbytes, ""
      m = Rubinius::Mirror.reflect str

      m.copy_from self, 0, bs, lbytes
      m.copy_from pad, 0, lbytes, 0
      m.copy_from pad, 0, rbytes, lbytes + bs
    else
      str = self.class.pattern width + bs, padding
      m = Rubinius::Mirror.reflect str
      m.copy_from self, 0, bs, left
    end

    str.taint if tainted? or padding.tainted?
    str.force_encoding enc
  end

  def ljust(width, padding=" ")
    padding = StringValue(padding)
    raise ArgumentError, "zero width padding" if padding.size == 0

    enc = Rubinius::Type.compatible_encoding self, padding

    width = Rubinius::Type.coerce_to width, Fixnum, :to_int
    return dup if width <= size

    width -= size

    bs = bytesize
    pbs = padding.bytesize

    if pbs > 1
      ps = padding.size
      pm = Rubinius::Mirror.reflect padding

      x = width / ps
      y = width % ps

      pbi = pm.byte_index(y)
      bytes = x * pbs + pbi

      str = self.class.pattern bytes + bs, self
      m = Rubinius::Mirror.reflect str

      i = 0
      bi = bs

      while i < x
        m.copy_from padding, 0, pbs, bi

        bi += pbs
        i += 1
      end

      m.copy_from padding, 0, pbi, bi
    else
      str = self.class.pattern width + bs, padding
      m = Rubinius::Mirror.reflect str

      m.copy_from self, 0, bs, 0
    end

    str.taint if tainted? or padding.tainted?
    str.force_encoding enc
  end

  def rjust(width, padding=" ")
    padding = StringValue(padding)
    raise ArgumentError, "zero width padding" if padding.size == 0

    enc = Rubinius::Type.compatible_encoding self, padding

    width = Rubinius::Type.coerce_to width, Fixnum, :to_int
    return dup if width <= size

    width -= size

    bs = bytesize
    pbs = padding.bytesize

    if pbs > 1
      ps = padding.size
      pm = Rubinius::Mirror.reflect padding

      x = width / ps
      y = width % ps

      bytes = x * pbs + pm.byte_index(y)
    else
      bytes = width
    end

    str = self.class.pattern bytes + bs, padding
    m = Rubinius::Mirror.reflect str

    m.copy_from self, 0, bs, bytes

    str.taint if tainted? or padding.tainted?
    str.force_encoding enc
  end

  def index(str, start=undefined)
    if undefined.equal?(start)
      start = 0
    else
      start = Rubinius::Type.coerce_to start, Fixnum, :to_int

      start += size if start < 0
      return if start < 0 or start > size
    end

    if str.kind_of? Regexp
      Rubinius::Type.compatible_encoding self, str

      m = Rubinius::Mirror.reflect self
      start = m.character_to_byte_index start
      if match = str.match_from(self, start)
        Regexp.last_match = match
        return match.begin(0)
      else
        Regexp.last_match = nil
        return
      end
    end

    str = StringValue(str)
    return start if str == ""

    Rubinius::Type.compatible_encoding self, str

    return if str.size > size

    m = Rubinius::Mirror.reflect self
    m.character_index str, start
  end

  def rindex(sub, finish=undefined)
    if undefined.equal?(finish)
      finish = size
    else
      finish = Rubinius::Type.coerce_to(finish, Integer, :to_int)
      finish += size if finish < 0
      return nil if finish < 0
      finish = size if finish >= size
    end

    m = Rubinius::Mirror.reflect self
    byte_finish = m.character_to_byte_index finish

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

      if byte_index = find_string_reverse(str, byte_finish)
        return m.byte_to_character_index byte_index
      end

    when Regexp
      Rubinius::Type.compatible_encoding self, sub

      match_data = sub.search_region(self, 0, byte_finish, false)
      Regexp.last_match = match_data
      return match_data.begin(0) if match_data

    else
      needle = StringValue(sub)
      needle_size = needle.size

      # needle is bigger that haystack
      return nil if size < needle_size

      # Boundary case
      return finish if needle_size == 0

      Rubinius::Type.compatible_encoding self, needle
      if byte_index = find_string_reverse(needle, byte_finish)
        return m.byte_to_character_index byte_index
      end
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
      multi_table = nil

      source.each_char do |chr|
        if chr.bytesize > 1
          multi_table ||= Hash.new { r }
          multi_table[chr] = -1
        else
          table[chr.ord] = -1
        end
      end
    else
      table = Rubinius::Tuple.pattern 256, -1
      multi_table = nil

      replacement.tr_expand! nil, false
      repl = replacement.__data__
      rsize = replacement.size
      i = 0
      source.each_char do |chr|
        repl_char = replacement[i]
        if repl_char && (chr.bytesize > 1 || repl_char.bytesize > 1)
          multi_table ||= {}
          multi_table[chr] = repl_char
        else
          r = repl[i] if i < rsize
          table[chr.ord] = r
        end
        i += 1
      end
    end

    self.modify!
    modified = false

    if squeeze
      last = nil

      i = 0
      byte_size = 0
      each_char do |chr|
        c = -1
        if chr.bytesize == 1
          c = table[chr.ord]
        end
        if c >= 0
          c_char = c.chr
          next if last == c_char
          byte_size += 1
          self[i] = c_char
          last = c_char
          modified = true
        elsif multi_table && (c = multi_table[chr])
          next if last == c
          self[i] = c
          last = c
          modified = true
          byte_size += c.bytesize
        else
          self[i] = chr
          byte_size += chr.bytesize
          last = nil
        end
        i += 1
      end

      self.num_bytes = byte_size if byte_size < @num_bytes
    else
      i = 0
      each_char do |chr|
        c = -1
        if chr.bytesize == 1
          c = table[chr.ord]
        end
        if c >= 0
          c_char = c.chr
          self[i] = c_char
          modified = true
        elsif multi_table && (c = multi_table[chr])
          self[i] = c
          modified = true
        end
        i += 1
      end
    end

    return modified ? self : nil
  end

  def <=>(other)
    if other.kind_of?(String)
      result = @data.compare_bytes(other.__data__, @num_bytes, other.bytesize)

      if result == 0
        if Encoding.compatible?(self, other)
          0
        else
          Rubinius::Type.encoding_order(encoding, other.encoding)
        end
      else
        result
      end
    else
      return unless other.respond_to?(:to_str) && other.respond_to?(:<=>)
      return unless tmp = (other <=> self)
      return -tmp # We're not supposed to convert to integer here
    end
  end
end

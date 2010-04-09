class Array::Packer
  BASE_64_B2A = Array.new 64
  (00..25).each {|x| BASE_64_B2A[x] = (?A + x - 00).chr}
  (26..51).each {|x| BASE_64_B2A[x] = (?a + x - 26).chr}
  (52..61).each {|x| BASE_64_B2A[x] = (?0 + x - 52).chr}
  BASE_64_B2A[62] = '+'
  BASE_64_B2A[63] = '/'

  POINTER_SIZE = Rubinius::L64 ? 8 : 4
  def initialize(array, schema)
    @source = array
    @schema = schema
    @index = 0
    @result = ""
    @ptr = nil
  end

  def endian?(order)
    order == Rubinius::ENDIAN
  end

  def ptr
    @ptr ||= FFI::MemoryPointer.new(16) # enough for all uses
  end

  def dispatch
    schema = @schema.gsub(/#.*/, '')
    i = 0
    length = schema.length
    native_size = false

    while i < length do
      kind = schema[i]
      i += 1

      next if kind.isspace

      if schema[i] == ?_ or schema[i] == ?!
        if kind == ?s or kind == ?S or kind == ?i or kind == ?I or kind == ?l or kind == ?L
          i += 1
          native_size = true
        else
          raise ArgumentError, "#{schema[i]} allowed only after types sSiIlL"
        end
      end

      if i == length
        len = 1
      elsif schema[i].isdigit
        len = 0
        begin
          len = len * 10 + schema[i] - ?0
          i += 1
        end while i < length and schema[i].isdigit
      elsif schema[i] == ?*
        len = case kind
              when ?@, ?X, ?x, ?u
                0
              when ?P, ?M, ?m
                1
              when ?u, ?m, ?H, ?h, ?B, ?b, ?A, ?a, ?Z
                nil  # size depends on item
              else
                @source.size - @index
              end
        i += 1
      else
        len = 1
      end
      case kind
      when ?X
        raise ArgumentError, "you're backing up too far" if len > @result.size
        @result.shorten!(len) if len > 0
      when ?x
        @result << "\000" * len
      when ?a, ?A, ?Z
        ascii_string(kind, len)
      when ?b, ?B
        bit_string(kind, len)
      when ?c, ?C
        character(kind, len)
      when ?M
        # for some reason MRI responds to to_s here
        item = Type.coerce_to(fetch_item(), String, :to_s)
        if len >= 3
          line_length = len
        else
          line_length = 72
        end
        line_length += 1 # bug compatibility with MRI

        @result << item.scan(/.{1,#{line_length}}/m).map { |line|
          line.gsub(/[^ -<>-~\t\n]/) { |m| "=%02X" % m[0] } + "=\n"
        }.join
      when ?m
        base64(len)
      when ?w
        ber_compress(kind, len)
      when ?u
        @result << uuencode(kind, len).join.gsub(/ /, '`')
      when ?d, ?D, ?e, ?E, ?f, ?F, ?g, ?G
        decimal(kind, len)
      when ?i, ?I, ?s, ?S, ?l, ?L, ?v, ?V
        integer(kind, len, native_size)
        native_size = false
      when ?N
        net_long(len)
      when ?n
        net_short(len)
      when ?p, ?P
        pointer(kind, len)
      when ?Q, ?q
        integer64(kind, len)
      when ?H, ?h
        hex_string(kind, len)
      when ?U
        utf_string(kind, len)
      when ?@
        pos = len
        if pos < @result.size
          @result = @result.substring(0, pos)
        elsif pos > @result.size
          @result << "\x00" * (pos - @result.size)
        end
      when ?%
        raise ArgumentError, "#{kind} not implemented"
      end
    end

    return @result
  ensure
    @ptr.free if @ptr
    @ptr = nil
  end

  def fetch_item
    raise ArgumentError, "too few array elements" if
      @index >= @source.length

    item = @source[@index]
    @index += 1
    return item
  end

  # A, a, Z
  def ascii_string(kind, size)
    item = fetch_item()
    # MRI nil compatibilty for string functions
    item = "" unless item

    item = StringValue(item)
    size = item.size + (kind == ?Z ? 1 : 0) unless size

    padsize = size - item.size
    filler  = kind == ?A ? " " : "\0"

    @result << item.substring(0, size)
    @result << filler * padsize if padsize > 0
  end

  # B, b
  def bit_string(kind, size)
    item = fetch_item()
    # MRI nil compatibilty for string functions
    item = "" unless item

    item = StringValue(item)
    byte = 0
    lsb  = (kind == ?b)
    size = item.size unless size

    min = size < item.size ? size : item.size

    if min > 0
      i = 0
      item.each_byte do |bit|
        bit &= 01
        j = i & 07
        byte |= bit << (lsb ? j : 07 - j)

        if j == 07
          @result << byte.chr
          byte = 0
        end

        i += 1
        break if i >= min
      end
    end

    # always output an incomplete byte
    if ((size & 07) != 0 || min != size) && item.size > 0
      @result << byte.chr
    end

    # Emulate the weird MRI spec for every 2 chars over output a \000 # FIX
    (item.length).step(size-1, 2) { |i| @result << 0 } if size > item.length
  end

  # C, c
  def character(kind, size)
    size.times do
      @result << (Type.coerce_to(fetch_item(), Integer, :to_int) & 0xff).chr
    end
  end

  # P, p
  def pointer(kind, size)
    size.times do
      item = fetch_item()
      unless item
        @result << "\x00" * POINTER_SIZE
      else
        item = StringValue(item)
        raise ArgumentError, "not implemented"
      end
    end
  end

  # H, h
  def hex_string(kind, size)
    item = StringValue(fetch_item())

    # MRI nil compatibilty for string functions
    item = "" unless item

    size = item.length unless size

    str = item.substring(0, size)
    tail = nil
    if kind == ?h
      0.step(str.length-2, 2) do |i|
        @result << "#{item[i+1].chr}#{item[i].chr}".hex.chr
      end
    else
      0.step(str.length-2, 2) do |i|
        @result << item.substring(i, 2).hex.chr
      end
    end
    tail = item.substring(str.length-1, 1).hex.chr if str.length % 2 == 1

    @result << (kind == ?H ? (tail[0]*16).chr : tail) if tail

    diff = size - item.length

    if diff > 0
      # we couldn't read as much data as was requested,
      # so we'll make up for that with a couple of zeroes.
      if (item.length % 2) == 0
        left = (diff / 2.0 + 0.5).to_i
      else
        left = diff / 2
      end

      @result << "\000" * left
    end
  end

  def decimal(kind, size)
    want_double = case kind
                  when ?E, ?D, ?d, ?G then true
                  when ?e, ?F, ?f, ?g then false
                  end

    little_endian = case kind
                    when ?e, ?E then true
                    when ?g, ?G then false
                    else endian?(:little)
                    end

    size.times do
      item = FloatValue(fetch_item())
      bytes = item.to_packed(want_double)
      bytes.reverse! if little_endian ^ endian?(:little)
      @result << bytes
    end
  end

  QUAD_MAX = 1 << 64
  MAX = 1 << Rubinius::WORDSIZE

  # Q, q
  def integer64(kind, size)
    size.times do |i|
      val = Type.coerce_to(fetch_item, Integer, :to_int)

      if val.abs >= QUAD_MAX
        raise RangeError, "bignum too big to convert into 'unsigned long'"
      end

      ptr.write_int64 val
      @result << ptr.read_string(8)
    end
  end

  # N
  def net_long(size)
    size.times do
      val = Type.coerce_to(fetch_item, Integer, :to_int)

      # These ranges checks are so stupid. They don't even check the actual
      # range of 32bit long!? But this is what MRI does.
      if val.abs >= MAX
        raise RangeError, "too big to be a network long"
      end

      ptr.write_int val
      ptr.network_order 0, 4
      @result << ptr.read_string(4)
    end
  end

  # n
  def net_short(size)
    size.times do
      val = Type.coerce_to(fetch_item, Integer, :to_int)

      # These ranges checks are so stupid. They don't even check the actual
      # range of short!? But this is what MRI does.
      if val.abs >= MAX
        raise RangeError, "too big to be a network short"
      end

      ptr.write_short val
      ptr.network_order 0, 2
      @result << ptr.read_string(2)
    end
  end

  # i, s, l, I, S, L, V, v
  def integer(kind, size, native_size)
    if native_size
      # Native sizes
      bytes = case kind
              when ?L, ?l then Rubinius::SIZEOF_LONG
              when ?I, ?i then Rubinius::SIZEOF_INT
              when ?S, ?s then Rubinius::SIZEOF_SHORT
              end
    else
      bytes = case kind
              when ?L, ?l, ?I, ?i, ?V then 4
              when ?S, ?s, ?v         then 2
              end
    end

    unsigned      = kind == ?I or kind == ?S or kind == ?L
    little_endian = case kind
                    when ?V, ?v then true
                    else endian?(:little)
                    end

    size.times do
      item = Type.coerce_to(fetch_item(), Integer, :to_int)

      if item.abs >= MAX
        raise RangeError, "bignum too big to convert into 'unsigned long'"
      end

      if little_endian
        item += 1 << (8 * bytes) if item < 0
        bytes.times { |b| @result << ((item >> (b * 8)) & 0xFF).chr }
      else # ugly
        @result << (0...bytes).map {n=(item & 0xFF).chr;item >>= 8; n}.reverse.join
      end
    end
  end

  # w
  def ber_compress(kind, size)
    size.times do
      item = Type.coerce_to(fetch_item(), Integer, :to_int)
      raise ArgumentError, "can't compress negative numbers" if item < 0

      chars = ''
      chars << (item & 0x7f)
      while (item >>= 7) > 0 do
        chars << ((item & 0x7f) | 0x80)
      end
      @result << chars.reverse
    end
  end

  # m
  def base64(line_length)
    line_length ||= 1
    @result << String.base64_encode(StringValue(fetch_item), line_length)
  end

  # u
  def uuencode(kind, size)
    item = StringValue(fetch_item())

    line_length = 45
    if size >= 3
      line_length = size
      line_length -= line_length % 3
    end

    # http://www.opengroup.org/onlinepubs/009695399/utilities/uuencode.html
    item.scan(/.{1,#{line_length}}/nm).map { |line|
      encoded = line.scan(/(.)(.?)(.?)/nm).map { |a,b,c|
        a = a[0]
        b = b[0] || 0
        c = c[0] || 0

        arr = [( a >> 2                    ) & 077,
               ((a << 4) | ((b >> 4) & 017)) & 077,
               ((b << 2) | ((c >> 6) & 003)) & 077,
               ( c                         ) & 077]
        arr.map {|x| (?\s+x).chr }
      }.flatten

      l = "#{encoded.join}\n"
      (line.size + ?\s).chr + l
    }
  end

  # U
  def utf_string(kind, size)
    size.times do
      item = Type.coerce_to(fetch_item(), Integer, :to_i)

      raise RangeError, "pack(U): value out of range" if item < 0

      bytes = 0

      f = [1 << 7, 1 << 11, 1 << 16, 1 << 21, 1 << 26, 1 << 31].find do |n|
        bytes += 1
        item < n
      end

      raise RangeError, "pack(U): value out of range" unless f

      if bytes == 1
        @result << item
      else
        i = bytes
        buf = []

        (i-1).times do
          buf.unshift(((item | 0x80) & 0xBF).chr)
          item >>= 6
        end
        # catch the highest bits - the mask depends on the byte count
        buf.unshift((item | ((0x3F00 >> bytes)) & 0xFC).chr)

        @result << buf.join
      end
    end
  end
end

class String::Unpacker
  BASE_64_A2B = {}
  (?A..?Z).each {|x| BASE_64_A2B[x] = x - ?A}
  (?a..?z).each {|x| BASE_64_A2B[x] = x - ?a + 26}
  (?0..?9).each {|x| BASE_64_A2B[x] = x - ?0 + 52}
  BASE_64_A2B[?+]  = ?>
  BASE_64_A2B[?\/] = ??
  BASE_64_A2B[?=]  = 0

  def initialize(source, schema)
    @source = source
    @length = source.length
    @schema = schema
  end

  def endian?(order)
    order == Rubinius::ENDIAN
  end

  def ptr
    @ptr ||= FFI::MemoryPointer.new(64) # enough for all uses
  end

  def parse_schema
    raise TypeError, "can't convert nil into String" unless @schema

    return @schema.scan(/([@a-zA-Z])(-?\d+|[\*_])?/).map { |c, n|
      # n in (nil, -num, num, "*", "_")
      [c, n.nil? || n =~ /\*|_/ ? n : Integer(n)]
    }
  end

  def stripped_string(i, count, elements)
    if i >= @length
      new_pos = i
      str = ''
    elsif count == '*'
      new_pos = @length
      str = @source[i..-1]
    else
      new_pos = i + count
      if new_pos <= @length
        str = @source[i...new_pos]
      else
        str = @source[i..-1]
      end
    end

    elements << str.sub(/[\x00\x20]+\Z/, '')
    return new_pos
  end

  def null_term_string(i, count, elements)
    if i >= @length
      elements << ''
    elsif count == '*'
      @source[i..-1] =~ / \A ( [^\x00]* ) ( [\x00]? ) /x
      elements << $1
      i += $1.length
      i += 1 if $2 == "\0"
    else
      str = i + count <= @length ? @source[i...(i + count)] : @source[i..-1]
      str =~ / \A ( [^\x00]* ) /x
      elements << $1
      i += count
    end

    return i
  end

  def string(i, count, elements)
    if i >= @length
      elements << ''
    elsif count == '*'
      elements << @source[i..-1]
      i = @length
    else
      nnd = i + count
      if i + count <= @length
        elements << @source[i...nnd]
      else
        elements << @source[i..-1]
      end
      i = nnd
    end

    return i
  end

  def bits_nibbles(code, i, count, elements)
    lsb = (code =~ /[bh]/)
    fmt = case code
          when /[Bb]/ then "%08b"
          when /[Hh]/ then "%02x"
          end

    if i >= @length
      elements << ''
    elsif count == '*'

      a = []
      @source[i..-1].each_byte do |b|
        a << (fmt % b)
      end

      a.map! { |s| s.reverse } if lsb
      elements << a.join
      i = @length
    else
      case code
      when /[Bb]/
        num_bytes, r = count.divmod(8)
        num_drop = r != 0 ? 8 - r : 0
      when /[Hh]/
        num_bytes, r = (count * 4).divmod(8)
        num_drop = r != 0 ? 1 : 0
      end
      num_bytes += 1 if r != 0
      str0 = if i + num_bytes <= @length
               @source[i...(i + num_bytes)]
             else
               @source[i..-1]
             end
      len = str0.length
      str1 = ''
      str0.each_byte do |n|
        len -= 1
        s = fmt % n
        if lsb
          str1 << (len == 0 ? s[num_drop..-1].reverse : s.reverse)
        else
          str1 << (len == 0 ? s[0..-num_drop.succ]    : s)
        end
      end

      elements << str1
      i += num_bytes
    end

    return i
  end

  INT_SIZE = FFI.type_size :int

  def int(i, count, elements, signed)
    # TODO honor _.
    int_size = INT_SIZE

    if count == "*"
      count = @length / int_size
    end

    count.times do
      if i + int_size <= @length
        ptr.write_string(@source[i, int_size], int_size)
        elements << ptr.read_int(signed)
      else
        elements << nil
      end

      i += int_size
    end

    return i
  end

  def ber_decompress(i, count, elements)
    count = @length - i if count == '*'

    count.times do
      n = 0
      while i < @length do
        item = @source[i]
        i += 1
        n <<= 7
        n |= item & 0x7f
        if item & 0x80 == 0
          elements << n
          break
        end
      end
    end

    return i
  end

  # Like int, but always does 4 bytes.
  def long(i, count, elements, signed)
    # TODO honor _.
    long_size = 4

    if count == "*"
      count = @length / long_size
    end

    count.times do
      if i + long_size <= @length
        ptr.write_string(@source[i, long_size], long_size)
        elements << ptr.read_int(signed)
      else
        elements << nil
      end

      i += long_size
    end

    return i
  end

  def number(code, i, count, elements)
    num_bytes = case code
                when /[DdEGQq]/ then 8
                when /[eFfgNV]/ then 4
                when /[nSsv]/   then 2
                when /[Cc]/     then 1
                end

    size = case code
           when /[NncGg]/          then :big
           when /[CILQSilqsFfDd]/ then :native
           when /[VveE]/          then :little
           else
             raise "huh? #{code.inspect}"
           end

    star = count == '*'
    count = @length - i if star

    count.times do |j|
      if i + num_bytes > @length
        break if star
        elements << nil if code != 'Q'
      else
        start    = i
        offset   = num_bytes - 1
        endian   = size
        n        = exp = 0
        bytebits = 8

        if :big == endian || (:native == endian && endian?(:big))
          exp      =  bytebits * offset
          bytebits = -bytebits
        end

        (start..start + offset).each do |x|
          n   += (@source[x] * 2**exp)
          exp += bytebits
        end

        case code
        when /[NnCILQSVv]/
          elements << n
        when /[ilqsc]/
          max = 2 ** (num_bytes * 8 - 1)
          n = n >= max ? -(2**(num_bytes*8) - n) : n
          elements << n
        when /[eFfg]/
          sign   = (2**31 & n != 0) ? -1 : 1
          expo   = ((0xFF * 2**23) & n) >> 23
          frac   = (2**23 - 1) & n
          result = if expo == 0 and frac == 0
                     sign.to_f * 0.0    # zero
                   elsif expo == 0 then # denormalized
                     sign * 2**(expo - 126) * (frac.to_f / 2**23.to_f)
                   elsif expo == 0xFF and frac == 0
                     sign.to_f / 0.0    # Infinity
                   elsif expo == 0xFF
                     0.0 / 0.0          # NaN
                   else                 # normalized
                     sign * 2**(expo - 127) * (1.0 + (frac.to_f / 2**23.to_f))
                   end
          elements << result
        when /[DdEG]/
          sign   = (2**63 & n != 0) ? -1 : 1
          expo   = ((0x7FF * 2**52) & n) >> 52
          frac   = (2**52 - 1) & n
          result = if expo == 0 and frac == 0
                     sign.to_f * 0.0    # zero
                   elsif expo == 0 then # denormalized
                     sign * 2**(expo - 1022) * (frac.to_f / 2**52.to_f)
                   elsif expo == 0x7FF and frac == 0
                     sign.to_f / 0.0    # Infinity
                   elsif expo == 0x7FF
                     0.0 / 0.0          # NaN
                   else                 # normalized
                     sign * 2**(expo-1023) * (1.0 + (frac.to_f / 2**52.to_f))
                   end
          elements << result
        end
        i += num_bytes
      end
    end

    return i
  end

  def quoted_printable(i, count, elements)
    if i >= @length
      elements << ''
    else
      bytes = i
      out = ""
      @source[i..-1].scan(/=(.?.?)|([^=]*)/nm) do |part|
        if equal = part[0]
          case equal.size
          when 0
            # nothing, ignore it.
          when 1
            if /^[A-Fa-f0-9]$/.match(equal)
              # consume and ignore.
              bytes += 2
            else
              # abort the whole thing if it's not a valid hex
              # character
              elements << out

              # The + 1 is to skip the =
              return bytes + 1
            end
          when 2
            if equal == "\r\n"
              # consume and ignore.
              bytes += 3
            elsif m = /^[A-Fa-f0-9]{1,2}/.match(equal)
              # See if we matched only one valid escape character,
              # in which case, ignore the sequence but don't consume
              # the unmatched character
              if m[0].size == 1
                elements << out
                return bytes + 2
              else
                # finally! convert it to base 16 then back into character.
                # Sadly, this is the most common case and it's buried.
                bytes += 3
                out << equal.to_i(16).chr
              end
            else
              # abort
              elements << out

              # The + 1 is to skip the =
              return bytes + 1
            end
          end
        else
          # A literal, non-equal-sign sequence. easy.
          lit = part[1]
          bytes += lit.size
          out << lit
        end
      end

      elements << out
      return bytes
    end

    return i
  end

  def base64(i, count, elements)
    if i >= @length
      elements << ''
    else
      buffer    = ''
      str       = ''
      num_bytes = 0

      b64_regex_permissive = /[A-Za-z0-9+\/]{4} |[A-Za-z0-9+\/]{3} \=?|[A-Za-z0-9+\/]{2}\={0,2} |[A-Za-z0-9+\/]\={0,3} |[^A-Za-z0-9+\/]+/x

      @source[i..-1].scan(b64_regex_permissive) do |s|
        num_bytes += s.length

        b64_regex_strict = /\A (?:[A-Za-z0-9+\/]{4} |[A-Za-z0-9+\/]{3} \=?|[A-Za-z0-9+\/]{2} \={0,2} |[A-Za-z0-9+\/] \={0,3} ) \Z /x

        if s =~ b64_regex_strict

          s << '=' while s.length != 4 if s =~ /=\Z/

            # TODO: WHY?
            if buffer == '' and s =~ /\A([A-Za-z0-9+\/])\=+\Z/
              buffer << $1
            else
              buffer << s
            end

          process = buffer.length >= 4

          if process
            s      = buffer[0..3]
            buffer = buffer[4..-1]

            a = BASE_64_A2B[s[0]]
            b = BASE_64_A2B[s[1]]
            c = BASE_64_A2B[s[2]]
            d = BASE_64_A2B[s[3]]

            # http://www.opengroup.org/onlinepubs/009695399/utilities/uuencode.html
            decoded = [a << 2 | b >> 4,
              (b & (2**4 - 1)) << 4 | c >> 2,
              (c & (2**2 - 1)) << 6 | d].pack('CCC')

            if s[3].chr == '='
              num_bytes -= 1
              decoded = decoded[0..-2]
              decoded = decoded[0..-2] if s[2].chr == '='
              str << decoded
              break
            else
              str << decoded
            end
          end
        end
      end
      elements << str
      i += num_bytes
    end

    return i

  end

  def utf8(i, count, elements)
    if count == "*"
      limit = nil
    else
      limit = count
    end

    while i < @length
      if limit
        break if limit == 0
        limit -= 1
      end

      c, len = @source.data.utf8_char(i)
      elements << c

      i += len
    end

    return i
  end

  def dispatch
    # some of the directives work when repeat == 0 because of this behavior:
    # str[0...0] == '' and str[1..0] == ''

    i = 0
    elements = []
    length = @source.length

    schema = parse_schema()

    schema.each do |code, count|

      count = nil if Fixnum === count and count < 0
      count ||= code == "@" ? 0 : 1

      # TODO: profile avg occurances and reorder case
      case code
      when 'A'
        i = stripped_string(i, count, elements)
      when 'a'
        i = string(i, count, elements)
      when 'B', 'b', 'H', 'h'
        i = bits_nibbles(code, i, count, elements)
      when 'i'
        i = int(i, count, elements, true)
      when 'I'
        i = int(i, count, elements, false)
      when 'l'
        i = long(i, count, elements, true)
      when 'L'
        i = long(i, count, elements, false)
      when /[CcDdEeFfGgNnQqSsVv]/
        i = number(code, i, count, elements)
      when 'M'
        i = quoted_printable(i, count, elements)
      when 'm'
        i = base64(i, count, elements)
      when 'U'
        i = utf8(i, count, elements)
      when 'w'
        i = ber_decompress(i, count, elements)
      when 'X'
        count = length - i if count == '*'

        raise ArgumentError, "X outside of string" if count < 0 or i - count < 0
        i -= count
      when 'x'
        if count == '*'
          raise ArgumentError, "x outside of string" if i > length
          i = length
        else
          raise ArgumentError, "x outside of string" if i + count > length
          i += count
        end
      when 'Z'
        i = null_term_string(i, count, elements)
      when '@'
        if count == '*'
          i = length
        else
          raise ArgumentError, "@ outside of string" if count > length
          i = count > 0 ? count : 0
        end
      else
        # raise "unknown directive: #{code.inspect}"
      end
    end

    elements

  ensure
    @ptr.free if @ptr
  end
end

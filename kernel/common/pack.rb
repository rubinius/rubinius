class Array::Packer
  BASE_64_B2A = Array.new 64
  (00..25).each {|x| BASE_64_B2A[x] = (?A + x - 00).chr}
  (26..51).each {|x| BASE_64_B2A[x] = (?a + x - 26).chr}
  (52..61).each {|x| BASE_64_B2A[x] = (?0 + x - 52).chr}
  BASE_64_B2A[62] = '+'
  BASE_64_B2A[63] = '/'

  POINTER_SIZE = Rubinius::L64 ? 8 : 4
  def initialize(array,schema)
    @source = array
    @schema = schema
    @index = 0
    @result = ""
  end

  def parse(schema)
    # The schema is an array of arrays like [["A", "6"], ["u", "*"],
    # ["X", ""]]. It represents the parsed form of "A6u*X".  Remove
    # strings in the schema between # and \n
    schema = schema.gsub(/#.*/, '')
    schema = schema.scan(/([^\s\d!_\*][\d!_\*]*)/).flatten.map {|x|
      x.match(/([^\s\d!_\*])([\d!_\*]*)/)[1..-1]
    }
  end

  def dispatch()
    parse(@schema).each do |kind, t|
      t = nil if t.empty?

      case kind # TODO: switch kind to ints
      when 'X' then
        size = (t || 1).to_i
        raise ArgumentError, "you're backing up too far" if size > @result.size
        @result[-size..-1] = '' if size > 0
      when 'x' then
        size = (t || 1).to_i
        @result << "\000" * size
      when 'a', 'A', 'Z' then
        ascii_string(kind, t)
      when 'b', 'B' then
        bit_string(kind, t)
      when 'c', 'C' then
        character(kind, t)
      when 'M' then
        # for some reason MRI responds to to_s here
        item = Type.coerce_to(fetch_item(), String, :to_s)
        # 75 chars per line includes =\n
        @result << item.scan(/.{1,73}/m).map { |line|
          line.gsub(/[^ -<>-~\t\n]/) { |m| "=%02X" % m[0] } + "=\n"
        }.join
      when 'm' then
        @result << encode(kind, t, :base64).join.sub(/(A{1,2})\n\Z/) { "#{'=' * $1.size}\n" }
      when 'w' then
        ber_compress(kind, t)
      when 'u' then
        @result << encode(kind, t, :uuencode).join.gsub(/ /, '`')
      when 'd', 'D', 'e', 'E', 'f', 'F', 'g', 'G' then
        decimal(kind, t)
      when 'i', 's', 'l', 'n', 'I', 'S', 'L', 'V', 'v', 'N', 'n' then
        integer(kind, t)
      when 'p', 'P' then
        pointer(kind, t)
      when 'Q', 'q' then
        item = Type.coerce_to(fetch_item(), Fixnum, :to_int)
        raise ArgumentError, "#{kind} not implemented"
      when 'H', 'h' then
        hex_string(kind, t)
      when 'U' then
        utf_string(kind, t)
      when '@'
        pos = if(t.nil?)
                raise ArgumentError
              else
                t.to_i
              end
        if(pos < @result.size)
          @result = @result[0...pos]
        elsif(pos > @result.size)
          @result << "\x00" * (pos - @result.size)
        end
      when '%'
        raise ArgumentError, "#{kind} not implemented"
      end
    end

    return @result
  end

  def fetch_item
    raise ArgumentError, "too few array elements" if
      @index >= @source.length

    item = @source[@index]
    @index += 1
    return item
  end

  def parse_tail(t, kind, remaining = @source.size - @index)
    case t
    when nil
      1
    when '*' then
      remaining
    else
      m = t.match(/(\d+)/)
      if(t.include?('_') || t.include?('!'))
        unless 'sSiIlL'.include?(kind)
          raise ArgumentError, "#{t} allowed only after types sSiIlL"
        end
      end
      m ? m[0].to_i : 1
    end
  end

  # A, a, Z
  def ascii_string(kind, t)
    item = fetch_item()
    # MRI nil compatibilty for string functions
    item = "" if item.nil?

    item = Type.coerce_to(item, String, :to_str)
    size = parse_tail(t, kind, item.size + (kind == "Z" ? 1 : 0))

    padsize = size - item.size
    filler  = kind == "A" ? " " : "\0"

    @result << item.split(//).first(size).join
    @result << filler * padsize if padsize > 0
  end

  # B, b
  def bit_string(kind, t)
    item = fetch_item()
    # MRI nil compatibilty for string functions
    item = "" if item.nil?

    item = Type.coerce_to(item, String, :to_str)
    byte = 0
    lsb  = (kind == "b")
    size = parse_tail(t, kind, item.size)

    bits = item.split(//).map { |c| c[0] & 01 }
    min = [size, item.size].min

    bits.first(min).each_with_index do |bit, i| # TODO: this can be cleaner
      i &= 07

      byte |= bit << (lsb ? i : 07 - i)

      if i == 07 then
        @result << byte.chr
        byte = 0
      end
    end

    # always output an incomplete byte
    if ((size & 07) != 0 || min != size) && item.size > 0 then
      @result << byte.chr
    end

    # Emulate the weird MRI spec for every 2 chars over output a \000 # FIX
    (item.length).step(size-1, 2) { |i| @result << 0 } if size > item.length
  end

  # C, c
  def character(kind, t)
    parse_tail(t, kind).times do
      @result << (Type.coerce_to(fetch_item(), Integer, :to_int) & 0xff).chr
    end
  end

  # P, p
  def pointer(kind, t)
    count = parse_tail(t, kind, kind == 'p' ? @source.size - @index : 1)
    raise ArgumentError, "too few array elements" if
      @index + count > @source.length && kind == 'p'

    count.times do
      item = fetch_item()
      if(item == nil)
        @result << "\x00"*POINTER_SIZE
      else
        item = StringValue item
        raise ArgumentError, "not implemented"
      end
    end
  end

  # H, h
  def hex_string(kind, t)
    item = Type.coerce_to(fetch_item(), String, :to_str)

    # MRI nil compatibilty for string functions
    item = "" if item.nil?

    size = parse_tail(t, kind, item.length)
    str = item[0, size].scan(/..?/)

    numbers = if kind == "h" then
                str.map { |b| b.reverse.hex }
              else
                str.map { |b| b.        hex }
              end

    if kind == 'H' && !numbers.empty? && numbers.last < 16
      numbers[-1] *= 16
    end

    diff = size - item.length

    if diff > 0
      # we couldn't read as much data as was requested,
      # so we'll make up for that with a couple of zeroes.
      if (item.length % 2) == 0
        left = (diff / 2.0 + 0.5).to_i
      else
        left = diff / 2
      end

      left.times do
        numbers << 0
      end
    end

    @result << numbers.map { |n| n.chr }.join
  end

  def decimal(kind, t)
    item = fetch_item()
    item = FloatValue item

    raise ArgumentError, "not implemented"
  end

  # i, s, l, n, I, S, L, V, v, N
  def integer(kind, t)
    size = parse_tail(t, kind)

    if(t && (t.include?('_') || t.include?('!')))
      native = t
    else
      native = false
    end

    unsigned      = (kind =~ /I|S|L/)
    little_endian = case kind
                    when 'V', 'v' then true
                    when 'N', 'n' then false
                    else endian?(:little)
                    end

    unless native then
      bytes = case kind
              when 'L', 'l' then 4
              when 'I', 'i' then 4
              when 'S', 's' then 2
              when 'V'      then 4
              when 'v'      then 2
              when 'N'      then 4
              when 'n'      then 2
              end
    end

    raise ArgumentError, "too few array elements" if
      @index + size > @source.length

    size.times do |i|
      item = Type.coerce_to(fetch_item(), Integer, :to_int)

      if item.abs >= 2**Rubinius::WORDSIZE
        raise RangeError, "bignum too big to convert into 'unsigned long'"
      end

      raise ArgumentError, "unsupported - fix me" if native

      @result << if little_endian then
                   item += 2 ** (8 * bytes) if item < 0
                   (0...bytes).map { |b| ((item >> (b * 8)) & 0xFF).chr }
                 else # ugly
                   (0...bytes).map {n=(item & 0xFF).chr;item >>= 8; n}.reverse
                 end.join
    end
  end

  # w
  def ber_compress(kind, t)
    item = Type.coerce_to(fetch_item(), Integer, :to_int)
    raise ArgumentError, "can't compress negative numbers" if item < 0

    @result << (item & 0x7f)
    while (item >>= 7) > 0 do
      @result << ((item & 0x7f) | 0x80)
    end

    @result.reverse! # FIX - breaks anything following BER?
  end

  # u, m
  def encode(kind, t, type = :base64)
    item = Type.coerce_to(fetch_item(), String, :to_str)

    # http://www.opengroup.org/onlinepubs/009695399/utilities/uuencode.html
    item.scan(/.{1,45}/m).map { |line|
      encoded = line.scan(/(.)(.?)(.?)/m).map { |a,b,c|
        a = a[0]
        b = b[0] || 0
        c = c[0] || 0

        arr = [( a >> 2                    ) & 077,
               ((a << 4) | ((b >> 4) & 017)) & 077,
               ((b << 2) | ((c >> 6) & 003)) & 077,
               ( c                         ) & 077]
        if(type == :uuencode)
          arr.map {|x| (?\s+x).chr }
        else
          arr.map {|x| BASE_64_B2A[x] }
        end
      }.flatten

      l = "#{encoded.join}\n"
      if(type == :uuencode)
        (line.size + ?\s).chr+l
      else
        l
      end
    }
  end

  # U
  def utf_string(kind, t)
    parse_tail(t, kind).times do
      item = Type.coerce_to(fetch_item(), Integer, :to_i)

      raise RangeError, "pack(U): value out of range" if item < 0

      bytes = 0
      f = [2 ** 7, 2 ** 11, 2 ** 16, 2 ** 21, 2 ** 26, 2 ** 31].find { |n|
        bytes += 1
        item < n
      }

      raise RangeError, "pack(U): value out of range" if f.nil?

      if bytes == 1 then
        @result << item
        bytes = 0
      end

      i = bytes

      buf = []
      if i > 0 then
        (i-1).times do
          buf.unshift((item | 0x80) & 0xBF)
          item >>= 6
        end
        # catch the highest bits - the mask depends on the byte count
        buf.unshift(item | ((0x3F00 >> bytes)) & 0xFC)
      end

      @result << buf.map { |n| n.chr }.join
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
    @schema = schema
  end

  def dispatch
    # some of the directives work when repeat == 0 because of this behavior:
    # str[0...0] == '' and str[1..0] == ''

    raise TypeError, "can't convert nil into String" if @schema.nil?

    i = 0
    elements = []
    length = @source.length

    schema = @schema.scan(/([@a-zA-Z])(-?\d+|[\*_])?/).map { |c, n|
      # n in (nil, -num, num, "*", "_")
      [c, n.nil? || n =~ /\*|_/ ? n : Integer(n)]
    }

    schema.each do |code, count|

      count = nil if Fixnum === count and count < 0
      count ||= code == "@" ? 0 : 1

      # TODO: profile avg occurances and reorder case
      case code
      when 'A' then
        new_pos, str = if i >= length then
                         [i, '']
                       elsif count == '*' then
                         [length, @source[i..-1]]
                       else
                         new_pos = i + count
                         [new_pos,
                          new_pos <= length ? @source[i...new_pos] : @source[i..-1]]
                       end
        i = new_pos
        elements << str.sub(/[\x00\x20]+\Z/, '')
      when 'a' then
        if i >= length then
          elements << ''
        elsif count == '*' then
          elements << @source[i..-1]
          i = length
        else
          nnd = i + count
          s = if i + count <= length then
                @source[i...nnd]
              else
                @source[i..-1]
              end
          elements << s
          i = nnd
        end
      when 'B', 'b', 'H', 'h' then
        lsb = code =~ /[bh]/
        fmt = case code
              when /[Bb]/ then "%08b"
              when /[Hh]/ then "%02x"
              end

        if i >= length then
          elements << ''
        elsif count == '*' then
          a = @source[i..-1].split(//).map { |c| fmt % c[0] }
          a.map! { |s| s.reverse } if lsb
          elements << a.join
          i = length
        else
          case code
          when /[Bb]/ then
            num_bytes, r = count.divmod(8)
            num_drop = r != 0 ? 8 - r : 0
          when /[Hh]/ then
            num_bytes, r = (count * 4).divmod(8)
            num_drop = r != 0 ? 1 : 0
          end
          num_bytes += 1 if r != 0
          str0 = if i + num_bytes <= length then
                   @source[i...(i + num_bytes)]
                 else
                   @source[i..-1]
                 end
          len = str0.length
          str1 = ''
          str0.each_byte { |n|
            len -= 1
            s = fmt % n
            str1 << if lsb then
                      len == 0 ? s[num_drop..-1].reverse : s.reverse
                    else
                      len == 0 ? s[0..-num_drop.succ]    : s
                    end
          }
          elements << str1
          i += num_bytes
        end
      when /[CcDdEeFfGgIiLlNnQqSsVv]/ then
        num_bytes = case code
                    when /[DdEGQq]/ then 8
                    when /[eFfgNV]/ then 4
                    when /[nSsv]/   then 2
                    when /[Cc]/     then 1
                    when /[IiLl]/   then 1.size
                    end

        size = case code
               when /[NncGg]/          then :big
               when /[CILQSilqsFfDd]/ then :native
               when /[VveE]/          then :little
               else
                 raise "huh? #{code.inspect}"
               end

        star = count == '*'
        count = length - i if star
        count.times do |j|
          if i + num_bytes > length
            break if star
            elements << nil if code != 'Q'
          else
            start    = i
            offset   = num_bytes - 1
            endian   = size
            n        = exp = 0
            bytebits = 8

            if :big == endian || (:native == endian && endian?(:big)) then
              exp      =  bytebits * offset
              bytebits = -bytebits
            end

            (start..start + offset).each do |x|
              n   += (@source[x] * 2**exp)
              exp += bytebits
            end

            case code
            when /[NnCILQSVv]/ then
              elements << n
            when /[ilqsc]/ then
              max = 2 ** (num_bytes * 8 - 1)
              n = n >= max ? -(2**(num_bytes*8) - n) : n
              elements << n
            when /[eFfg]/ then
              sign   = (2**31 & n != 0) ? -1 : 1
              expo   = ((0xFF * 2**23) & n) >> 23
              frac   = (2**23 - 1) & n
              result = if expo == 0 and frac == 0 then
                         sign.to_f * 0.0    # zero
                       elsif expo == 0 then # denormalized
                         sign * 2**(expo - 126) * (frac.to_f / 2**23.to_f)
                       elsif expo == 0xFF and frac == 0 then
                         sign.to_f / 0.0    # Infinity
                       elsif expo == 0xFF then
                         0.0 / 0.0          # NaN
                       else                 # normalized
                         sign * 2**(expo - 127) * (1.0 + (frac.to_f / 2**23.to_f))
                       end
              elements << result
            when /[DdEG]/ then
              sign   = (2**63 & n != 0) ? -1 : 1
              expo   = ((0x7FF * 2**52) & n) >> 52
              frac   = (2**52 - 1) & n
              result = if expo == 0 and frac == 0 then
                         sign.to_f * 0.0    # zero
                       elsif expo == 0 then # denormalized
                         sign * 2**(expo - 1022) * (frac.to_f / 2**52.to_f)
                       elsif expo == 0x7FF and frac == 0 then
                         sign.to_f / 0.0    # Infinity
                       elsif expo == 0x7FF then
                         0.0 / 0.0          # NaN
                       else                 # normalized
                         sign * 2**(expo-1023) * (1.0 + (frac.to_f / 2**52.to_f))
                       end
              elements << result
            end
            i += num_bytes
          end
        end
      when 'M' then
        if i >= length then
          elements << ''
        else
          str              = ''
          num_bytes        = 0
          regex_permissive = / \= [0-9A-Fa-f]{2} | [^=]+ | [\x00-\xFF]+ /x
          regex_junk       = / \A ( \= [0-9A-Fa-f]{0,1} )               /x
          regex_strict     = / \A (?: \= [0-9A-Fa-f]{2} | [^=]+ )    \Z /x
          regex_hex        = / \A \= ([0-9A-Fa-f]{2})                \Z /x

          @source[i..-1].scan(regex_permissive) do |s|
            if s =~ regex_strict then
              num_bytes += s.length
              s = $1.hex.chr if s =~ regex_hex
              str << s
            elsif s =~ regex_junk
              num_bytes += $1.length
            end
          end

          elements << str
          i += num_bytes
        end
      when 'm' then
        if i >= length
          elements << ''
        else
          buffer    = ''
          str       = ''
          num_bytes = 0

          b64_regex_permissive = /[A-Za-z0-9+\/]{4} |[A-Za-z0-9+\/]{3} \=?
            |[A-Za-z0-9+\/]{2}\={0,2} |[A-Za-z0-9+\/]\={0,3} |[^A-Za-z0-9+\/]+/x

          @source[i..-1].scan(b64_regex_permissive) do |s|
            num_bytes += s.length

            b64_regex_strict = /\A (?:[A-Za-z0-9+\/]{4} |[A-Za-z0-9+\/]{3} \=?
              |[A-Za-z0-9+\/]{2} \={0,2} |[A-Za-z0-9+\/] \={0,3} ) \Z /x

            if s =~ b64_regex_strict then

              s << '=' while s.length != 4 if s =~ /=\Z/

                # TODO: WHY?
                if buffer == '' and s =~ /\A([A-Za-z0-9+\/])\=+\Z/ then
                  buffer << $1
                else
                  buffer << s
                end

              process = buffer.length >= 4

              if process then
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
      when 'U' then
        utf8_regex_strict = /\A(?:[\x00-\x7F]
                                 |[\xC2-\xDF][\x80-\xBF]
                                 |[\xE1-\xEF][\x80-\xBF]{2}
                                 |[\xF1-\xF7][\x80-\xBF]{3}
                                 |[\xF9-\xFB][\x80-\xBF]{4}
                                 |[\xFD-\xFD][\x80-\xBF]{5} )\Z/x

        utf8_regex_permissive = / [\x00-\x7F]
                                 |[\xC0-\xDF][\x80-\xBF]
                                 |[\xE0-\xEF][\x80-\xBF]{2}
                                 |[\xF0-\xF7][\x80-\xBF]{3}
                                 |[\xF8-\xFB][\x80-\xBF]{4}
                                 |[\xFC-\xFD][\x80-\xBF]{5}
                                 |[\x00-\xFF]+ /x

        if i >= length then
          # do nothing?!?
        else
          num_bytes = 0
          @source[i..-1].scan(utf8_regex_permissive) do |c|
            raise ArgumentError, "malformed UTF-8 character" if
              c !~ utf8_regex_strict

            break if count == 0

            if false then
              elements << c.utf8_code_value
              num_bytes += c.length
            else
              len = c.length
              if len == 1
                result = c[0]
              else
                shift = (len - 1) * 2
                result = (((2 ** (8 - len.succ) - 1) & c[0]) *
                          2 ** ((len - 1) * 8)) >> shift
                (1...(len - 1)).each do |x|
                  shift -= 2
                  result |= (((2 ** 6 - 1) & c[x]) *
                             2 ** ((len - x.succ) * 8)) >> shift
                end
                result |= (2 ** 6 - 1) & c[-1]
              end
              elements << result
              num_bytes += len
            end

            count -= 1 if count != '*'
          end
          i += num_bytes
        end
      when 'X' then
        count = length - i if count == '*'

        raise ArgumentError, "X outside of string" if count < 0 or i - count < 0
        i -= count
      when 'x' then
        if count == '*' then
          raise ArgumentError, "x outside of string" if i > length
          i = length
        else
          raise ArgumentError, "x outside of string" if i + count > length
          i += count
        end
      when 'Z' then
        if i >= length then
          elements << ''
        elsif count == '*' then
          @source[i..-1] =~ / \A ( [^\x00]* ) ( [\x00]? ) /x
          elements << $1
          i += $1.length
          i += 1 if $2 == "\0"
        else
          str = i + count <= length ? @source[i...(i + count)] : @source[i..-1]
          str =~ / \A ( [^\x00]* ) /x
          elements << $1
          i += count
        end
      when '@' then
        if count == '*' then
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
  end
end

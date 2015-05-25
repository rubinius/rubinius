module Rubinius

  # CAPI contains utility methods used by capi.
  module CAPI
    def self.coerce(x, y, error=false)
      begin
        ary = y.coerce x
      rescue
        if error
          raise TypeError, "#{y.inspect} can't be coerced into #{x.class}"
        else
          return false
        end
      end

      unless ary.kind_of? Array and ary.size == 2
        if error
          raise TypeError, "coerce must return [x, y]"
        else
          return false
        end
      end

      return ary
    end

    # C-API method corresponding to rb_num_coerce_bin
    def self.rb_num_coerce_bin(x, y, op)
      a, b = coerce x, y, true

      a.__send__ op, b
    end

    # C-API method corresponding to rb_num_coerce_cmp
    def self.rb_num_coerce_cmp(x, y, op)
      ary = coerce x, y

      return ary.first.__send__ op, ary.last if ary
    end

    # C-API method corresponding to rb_num_coerce_relop
    def self.rb_num_coerce_relop(x, y, op)
      ary = coerce x, y, false

      if ary
        rel = ary.first.__send__ op, ary.last
        return rel unless rel.equal? nil
      end

      raise ArgumentError, "comparison of #{x.class} with #{y.class} failed"
    end

    def self.sprintf(format, args)
      ::Rubinius::Sprinter.get(format).call(*args)
    end

    def self.rb_str_encode(str, enc, flags, opts)
      # TODO: set flags from opts
      if str.encoding == enc
        str.dup
      else
        ec = Encoding::Converter.new str.encoding, enc, flags
        ec.replacement = opts[:replace] if opts

        ec.convert str
      end
    end

    def self.rb_str_conv_enc_opts(str, from, to, flags, opts)
      return str if to.nil?
      return str if from == to

      if (to.ascii_compatible? and str.ascii_only?) or to == Encoding::ASCII_8BIT
        if str.encoding != to
          return str.dup.force_encoding(to)
        else
          return str
        end
      end

      # TODO: set flags from opts
      ec = Encoding::Converter.new from, to, flags
      ec.replacement = opts[:replace] if opts

      result = ""
      status = ec.primitive_convert str.dup, result, 0, nil, flags

      return status == :finished ? result : str
    end

    INTEGER_PACK_MSWORD_FIRST       = 0x01
    INTEGER_PACK_LSWORD_FIRST       = 0x02
    INTEGER_PACK_MSBYTE_FIRST       = 0x10
    INTEGER_PACK_LSBYTE_FIRST       = 0x20
    INTEGER_PACK_NATIVE_BYTE_ORDER  = 0x40
    INTEGER_PACK_2COMP              = 0x80
    INTEGER_PACK_FORCE_BIGNUM       = 0x100
    INTEGER_PACK_NEGATIVE           = 0x200

    INTEGER_PACK_LITTLE_ENDIAN  = INTEGER_PACK_LSWORD_FIRST|INTEGER_PACK_LSBYTE_FIRST
    INTEGER_PACK_BIG_ENDIAN     = INTEGER_PACK_MSWORD_FIRST|INTEGER_PACK_MSBYTE_FIRST

    INTEGER_PACK_DIGITS = [
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ]

    def self.rb_integer_pack(value, words, numwords, wordsize, nails, flags)
      digits = INTEGER_PACK_DIGITS
      numbytes = numwords * wordsize
      nibble_size = numbytes * 2

      nibbles = value.abs.to_s(16)
      nibbles = nibbles[-nibble_size..-1] if nibble_size < nibbles.size

      complement = value < 0 && (flags & INTEGER_PACK_2COMP) != 0

      if (ENDIAN == :little and (flags & INTEGER_PACK_BIG_ENDIAN) != 0) or
         (ENDIAN == :big and (flags & INTEGER_PACK_LITTLE_ENDIAN) != 0)
        c_index = index = numbytes - 1
        incr = -1
      else
        c_index = index = 0
        incr = 1
      end

      i = nibbles.size - 1
      stop = nibbles.size & 1

      while i > stop
        high = digits[nibbles.getbyte(i - 1)]
        low = digits[nibbles.getbyte(i)]
        byte = (high << 4) | low

        byte ^= 0xff if complement
        words.setbyte index, byte

        index += incr
        i -= 2
      end

      if stop == 1
        byte = digits[nibbles.getbyte(0)]
        byte ^= 0xff if complement
        words.setbyte index, byte
      end

      if complement
        c_stop = c_index == 0 ? numbytes : -1

        while c_index != c_stop
          byte = words.getbyte(c_index) + 1
          words.setbyte(c_index, byte)

          break if byte <= 0xff

          c_index += incr
        end
      end

      overflow = 2**(numwords * wordsize * 8)

      if value < 0
        if complement
          return -2 if value < -overflow
        else
          return -2 if value <= -overflow
        end
        return -1
      else
        return 2 if value >= overflow
        return 1
      end
    end

    def self.rb_block_proc(env)
      Rubinius::Mirror::Proc.from_block ::Proc, env
    end
  end
end

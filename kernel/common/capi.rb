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
  end
end

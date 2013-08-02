# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.coerce_to_encoding(obj)
      case obj
      when Encoding
        return obj
      when String
        return Encoding.find obj
      else
        return Encoding.find StringValue(obj)
      end
    end

    def self.try_convert_to_encoding(obj)
      case obj
      when Encoding
        return obj
      when String
        str = obj
      else
        str = StringValue obj
      end

      key = str.upcase.to_sym

      pair = Encoding::EncodingMap[key]
      if pair
        index = pair.last
        return index && Encoding::EncodingList[index]
      end

      return undefined
    end

    def self.coerce_to_path(obj)
      if object_kind_of?(obj, String)
        obj
      else
        if object_respond_to? obj, :to_path
          obj = obj.to_path
        end

        StringValue(obj)
      end
    end

    def self.coerce_to_symbol(obj)
      return obj if object_kind_of? obj, Symbol

      obj = obj.to_str if obj.respond_to?(:to_str)
      coerce_to(obj, Symbol, :to_sym)
    end

    def self.ivar_validate(name)
      # adapted from rb_to_id
      case name
      when String
        return name.to_sym if name[0] == ?@
      when Symbol
        return name if name.is_ivar?
      else
        name = Rubinius::Type.coerce_to(name, String, :to_str)
        return name.to_sym if name[0] == ?@
      end

      raise NameError, "`#{name}' is not allowed as an instance variable name"
    end

    def self.coerce_to_binding(obj)
      if obj.kind_of? Binding
        binding = obj
      elsif obj.kind_of? Proc
        raise TypeError, 'wrong argument type Proc (expected Binding)'
      elsif obj.respond_to? :to_binding
        binding = obj.to_binding
      else
        binding = obj
      end

      unless binding.kind_of? Binding
        raise ArgumentError, "unknown type of binding"
      end

      binding
    end

    # Equivalent of num_exact in MRI's time.c; used by Time methods.
    def self.coerce_to_exact_num(obj)
      if obj.kind_of?(Integer)
        obj
      elsif obj.kind_of?(String)
        raise TypeError, "can't convert #{obj} into an exact number"
      elsif obj.nil?
        raise TypeError, "can't convert nil into an exact number"
      else
        check_convert_type(obj, Rational, :to_r) || coerce_to(obj, Integer, :to_int)
      end
    end

    def self.coerce_to_utc_offset(offset)
      offset = String.try_convert(offset) || offset

      if offset.kind_of?(String)
        unless offset.encoding.ascii_compatible? && offset.match(/\A(\+|-)(\d\d):(\d\d)\z/)
          raise ArgumentError, '"+HH:MM" or "-HH:MM" expected for utc_offset'
        end

        offset = $2.to_i*60*60 + $3.to_i*60
        offset = -offset if $1.ord == 45
      else
        offset = Rubinius::Type.coerce_to_exact_num(offset)
      end

      if offset <= -86400 || offset >= 86400
        raise ArgumentError, "utc_offset out of range"
      end

      offset
    end

    def self.coerce_to_pid(obj)
      Rubinius::Type.coerce_to obj, Integer, :to_int
    end

    def self.object_initialize_dup(obj, copy)
      Rubinius.privately do
        copy.initialize_dup obj
      end
    end

    def self.object_initialize_clone(obj, copy)
      Rubinius.privately do
        copy.initialize_clone obj
      end
    end

    def self.object_respond_to_ary?(obj)
      object_respond_to?(obj, :to_ary, true)
    end

    def self.binary_string(string)
      string.force_encoding Encoding::BINARY
    end

    def self.external_string(string)
      string.force_encoding Encoding.default_external
    end

    def self.encode_string(string, enc)
      string.force_encoding enc
    end

    def self.ascii_compatible_encoding(string)
      compatible_encoding string, Encoding::US_ASCII
    end

    def self.compatible_encoding(a, b)
      enc = Encoding.compatible? a, b

      unless enc
        enc_a = object_encoding a
        enc_b = object_encoding b
        message = "undefined conversion "
        message << "for '#{a.inspect}' " if object_kind_of?(a, String)
        message << "from #{enc_a} to #{enc_b}"

        raise Encoding::CompatibilityError, message
      end

      enc
    end

    def self.encoding_order(a, b)
      index_a = Encoding::EncodingMap[a.name.upcase][1]
      index_b = Encoding::EncodingMap[b.name.upcase][1]
      index_a <=> index_b
    end
  end
end

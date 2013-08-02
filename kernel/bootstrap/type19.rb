# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.coerce_to_array(obj)
      return [obj] unless obj

      return Rubinius.privately { obj.to_a } if obj.respond_to?(:to_a, true)
      return obj.to_ary if obj.respond_to?(:to_ary)

      # On 1.9, #to_a is not defined on all objects, so wrap the object in a
      # literal array.
      return [obj]
    end

    def self.coerce_to_float(obj, strict=true, must_be_numeric=true)
      if !must_be_numeric && object_kind_of?(obj, String)
        value = Rubinius.invoke_primitive :string_to_f, obj, strict
        raise ArgumentError, "invalid value for Float" if value.nil?
        return value
      end

      case obj
      when Numeric
        coerce_to obj, Float, :to_f
      when Float
        return obj
      when nil, true, false
        raise TypeError, "can't convert #{obj.inspect} into Float"
      else
        raise TypeError, "can't convert #{obj.class} into Float"
      end
    end

    def self.object_encoding(obj)
      Rubinius.primitive :encoding_get_object_encoding
      raise PrimitiveFailure, "Rubinius::Type.object_encoding primitive failed"
    end

    def self.convert_to_names(list)
      list
    end

    def self.convert_to_name(sym)
      sym
    end
  end
end

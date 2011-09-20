module Rubinius
  module Type
    def self.coerce_to_path(obj)
      unless object_kind_of?(obj, String)
        if converted = try_convert(obj, String, :to_path)
          obj = converted
        else
          obj = StringValue(obj)
        end
      end
      obj
    end

    def self.coerce_to_float(obj)
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
  end
end

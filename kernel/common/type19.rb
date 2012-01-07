# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.coerce_to_encoding(obj)
      case obj
      when Encoding
        return obj
      when String
        return Encoding.find obj
      when nil
        # TODO: temporary until __ENCODING__ is fixed
      else
        return Encoding.find StringValue(obj)
      end
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

    def self.coerce_to_symbol(obj)
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

    # Taint host if source is tainted.
    # Untrust host if source is untrusted.
    def self.infect(host, source)
      host.taint if source.tainted?
      host.untrust if source.untrusted?
    end
  end
end

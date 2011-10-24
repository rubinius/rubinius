module Rubinius
  module Type
    def self.coerce_to_path(obj)
      StringValue(obj)
    end

    def self.coerce_to_float(obj)
      Float(obj)
    end

    def self.coerce_to_symbol(obj)
      if object_kind_of?(obj, Fixnum)
        raise ArgumentError, "Fixnums (#{obj}) cannot be used as symbols"
      end
      obj = obj.to_str if obj.respond_to?(:to_str)

      coerce_to(obj, Symbol, :to_sym)
    end
  end
end

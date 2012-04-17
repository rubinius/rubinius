# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.coerce_to_array(obj)
      return [obj] unless obj

      return obj.to_ary if obj.respond_to?(:to_ary)

      # Just call #to_a, which wraps the reciever in an
      # array if it's not one.
      return obj.to_a
    end

    def self.coerce_to_float(obj, strict=true, must_be_numeric=true)
      case obj
      when String
        value = Rubinius.invoke_primitive :string_to_f, obj, strict
        raise ArgumentError, "invalid value for Float" if value.nil?
        value
      else
        Float(obj)
      end
    end
  end
end

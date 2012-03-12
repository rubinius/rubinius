# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.coerce_to_array(obj)
      return [obj] unless obj

      return obj.to_a if obj.respond_to?(:to_a)
      return obj.to_ary if obj.respond_to?(:to_ary)

      # On 1.9, #to_a is not defined on all objects, so wrap the object in a
      # literal array.
      return [obj]
    end

    def self.object_encoding(obj)
      Rubinius.primitive :encoding_get_object_encoding
      raise PrimitiveFailure, "Rubinius::Type.object_encoding primitive failed"
    end
  end
end

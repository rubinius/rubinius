# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.object_encoding(obj)
      Rubinius.primitive :encoding_get_object_encoding
      raise PrimitiveFailure, "Rubinius::Type.object_encoding primitive failed"
    end
  end
end

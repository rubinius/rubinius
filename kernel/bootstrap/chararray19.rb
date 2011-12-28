module Rubinius
  class CharArray
    attr_writer :encoding
    attr_reader :ascii
    attr_reader :valid

    def encoding
      Rubinius.primitive :chararray_encoding
      raise PrimitiveFailure, "CharArray#encoding primitive failed"
    end

    def ascii_only?(bytes)
      Rubinius.primitive :chararray_ascii_only_p
      raise PrimitiveFailure, "CharArray#ascii_only? primitive failed"
    end

    def valid_encoding?(bytes)
      Rubinius.primitive :chararray_valid_encoding_p
      raise PrimitiveFailure, "CharArray#valid_encoding? primitive failed"
    end
  end
end

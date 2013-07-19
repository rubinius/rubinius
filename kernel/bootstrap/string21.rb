# -*- encoding: us-ascii -*-

class String
  attr_writer :encoding
  attr_writer :ascii_only
  attr_writer :valid_encoding

  def self.from_codepoint(code, enc)
    Rubinius.primitive :string_from_codepoint
    raise PrimitiveFailure, "String.from_codepoint primitive failed"
  end

  def ascii_only?
    Rubinius.primitive :string_ascii_only_p
    raise PrimitiveFailure, "String#ascii_only? primitive failed"
  end

  def encoding
    Rubinius.primitive :string_encoding
    raise PrimitiveFailure, "String#encoding primitive failed"
  end

  def ord
    Rubinius.primitive :string_codepoint
    raise ArgumentError, 'empty string' if empty?
    raise ArgumentError, "invalid byte sequence in #{encoding}"
  end

  def chr_at(byte)
    Rubinius.primitive :string_chr_at
    raise ArgumentError, "String#chr_at primitive failed"
  end

  def valid_encoding?
    Rubinius.primitive :string_valid_encoding_p
    raise PrimitiveFailure, "String#valid_encoding? primitive failed"
  end
end

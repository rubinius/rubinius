class String
  def self.from_codepoint(code, enc)
    Rubinius.primitive :string_from_codepoint
    raise PrimitiveFailure, "String.from_codepoint primitive failed"
  end
end

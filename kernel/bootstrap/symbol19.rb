# -*- encoding: us-ascii -*-

class Symbol
  def encoding
    Rubinius.primitive :symbol_encoding
    raise PrimitiveFailure, "Symbol#encoding primitive failed"
  end
end

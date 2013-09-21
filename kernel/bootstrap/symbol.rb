class Symbol
  def self.===(obj)
    Rubinius.primitive :symbol_s_eqq
    super
  end

  def encoding
    Rubinius.primitive :symbol_encoding
    raise PrimitiveFailure, "Symbol#encoding primitive failed"
  end

  def index
    Rubinius.primitive :symbol_index
    raise PrimitiveFailure, "Symbol#index primitive failed."
  end

  def is_ivar?
    Rubinius.primitive :symbol_is_ivar
    raise PrimitiveFailure, "Symbol#is_ivar primitive failed."
  end

  def is_cvar?
    Rubinius.primitive :symbol_is_cvar
    raise PrimitiveFailure, "Symbol#is_cvar primitive failed."
  end

  def is_constant?
    Rubinius.primitive :symbol_is_constant
    raise PrimitiveFailure, "Symbol#is_constant primitive failed."
  end
end

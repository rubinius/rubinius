class Symbol
  def self.===(obj)
    Ruby.primitive :symbol_s_eqq
    super
  end

  def index
    Ruby.primitive :symbol_index
    raise PrimitiveFailure, "Symbol#index failed."
  end

  def is_ivar?
    Ruby.primitive :symbol_is_ivar
    raise PrimitiveFailure, "Symbol#is_ivar failed."
  end

  def is_cvar?
    Ruby.primitive :symbol_is_cvar
    raise PrimitiveFailure, "Symbol#is_cvar failed."
  end

  def is_constant?
    Ruby.primitive :symbol_is_constant
    raise PrimitiveFailure, "Symbol#is_constant failed."
  end
end

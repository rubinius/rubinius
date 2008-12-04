class Symbol
  def index
    Ruby.primitive :symbol_index
    raise PrimitiveFailure, "Symbol#index failed."
  end

  def to_s
    Ruby.primitive :symbol_to_s
    raise PrimitiveFailure, "Symbol#to_s failed."
  end

  def to_sym
    self
  end

  def is_ivar?
    Ruby.primitive :symbol_is_ivar
    raise PrimitiveFailure, "Symbol#is_ivar failed."
  end
end

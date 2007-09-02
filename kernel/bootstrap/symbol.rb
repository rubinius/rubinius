class SymbolTable
  ivar_as_index :symbols => 1
  
  def symbol_to_string(sym)
    @symbols.at(sym.index)  
  end
end

class Symbol
  def index
    Ruby.primitive :symbol_index
    raise PrimitiveFailure, "Symbol#index failed."
  end
  
  def to_s
    Symbols.symbol_to_string(self)
  end
  
  def to_sym
    self
  end
end

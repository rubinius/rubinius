class SymbolTable
  def symbols
    @symbols
  end
  
  def strings
    @strings
  end
  
  def symbol_to_string(sym)
    @symbols.at(sym.index)
  end  
end

class Symbol
  def index
    Ruby.primitive :symbol_index
  end
  
  def to_s
    Symbols.symbol_to_string(self)
  end
  
  def inspect
    ":#{to_s}"
  end
  
  def to_sym
    self
  end
end
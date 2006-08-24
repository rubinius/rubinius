class SymbolTable
  def symbols
    at(1)
  end
  
  def strings
    at(2)
  end
  
  def symbol_to_string(sym)
    symbols.at(sym.index)
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
end
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
  
  alias :to_i   :index
  alias :to_int :index
  
  def to_s
    Symbols.symbol_to_string(self)
  end
  
  alias :id2name :to_s
  
  def inspect
    ":#{to_s}"
  end
  
  def to_sym
    self
  end
end
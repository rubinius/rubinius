class SymbolTable
  ivar_as_index :__ivars__ => 0, :symbols => 1, :strings => 2
  def __ivars__; @__ivars__ ; end
  def symbols  ; @symbols   ; end
  def strings  ; @strings   ; end
  
  def symbol_to_string(sym)
    @symbols.at(sym.index).dup
  end  
end

class Symbol
  def self.all_symbols
    # Symbols.strings.values.map {|x| x.to_sym}
    Symbols.symbols.to_a.map { |x| x.intern }
  end

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


class SymbolTable
  ivar_as_index :__ivars__ => 0, :symbols => 1, :strings => 2
  def __ivars__; @__ivars__ ; end
  def symbols  ; @symbols   ; end
  def strings  ; @strings   ; end  
end

class Symbol
  def self.all_symbols
    # Symbols.strings.values.map {|x| x.to_sym}
    Symbols.symbols.to_a.map { |x| x.intern }
  end
  
  alias_method :to_i,   :index
  alias_method :to_int, :index
    
  alias_method :id2name, :to_s
  
  def inspect
    ":#{to_s}"
  end
  
  def to_sym
    self
  end
  
  def to_s
    Symbols.symbol_to_string(self).dup
  end
  
  def to_int
    warn "treating Symbol as an integer"
    self.to_i
  end
end
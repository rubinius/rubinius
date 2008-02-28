# depends on: class.rb

class SymbolTable
  ivar_as_index :__ivars__ => 0, :symbols => 1, :strings => 2
  def __ivars__; @__ivars__ ; end
  def symbols  ; @symbols   ; end
  def strings  ; @strings   ; end
end

class Symbol
  def self.all_symbols
    # The find_all prunes nil out of the Tuple
    Symbols.symbols.find_all { |s| s }.map { |x| x.to_sym }
  end

  def inspect
    str = to_s
    case str
    when /^(!|!=|!~|-@|\+@|$|%|&|&&|'|\*|\*\*|,|\.|\.\.|\.\.\.|\/|:|::|;|<|<=|<=>|=|==|===|=>|=~|>|>=|>>|\?|@|\[\]|\[\]=|\"|<<|\||\|\||^|`|~)/ then
      ":#{str}"
    when /^($-?|@@?)?[a-z_]+[?!]?/ then # TODO: verify this is correct
      ":#{str}"
    else
      ":\"#{str}\""
    end
  end

  def to_sym
    self
  end

  alias_method :intern, :to_sym

  def to_s
    Symbols.symbol_to_string(self).dup
  end

  alias_method :id2name, :to_s
end

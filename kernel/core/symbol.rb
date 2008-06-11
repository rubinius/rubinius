# depends on: class.rb

##
# Stores all of the Symbols.

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
    str = self.to_s

    case str
    when /^(\$|@@?)[a-z_][a-z_\d]*$/i,                     # Variable names
         /^[a-z_][a-z_\d]*[?!]?$/i,                        # Method names
         /^\$(-[a-z_\d]|[+~:?<_\/'"$.,`!;\\=*>&@]|\d+)$/i, # Special global variables
         /^([|^&\/%~`]|<<|>>|<=>|===?|=~|[<>]=?|[+-]@?|\*\*?|\[\]=?)$/ # Operators
      ":#{str}"
    else
      ":#{str.inspect}"
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

  # Slightly modified from ActiveSupport to avoid a reliance on left-to-right
  # evaluation order
  def to_proc
    Proc.new do |*args|
      obj = args.shift
      if args.empty? then
        obj.__send__(self)
      else
        obj.__send__(self, *args)
      end
    end
  end
end

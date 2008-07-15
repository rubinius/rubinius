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
  ##
  # Returns an array of all the symbols currently in Ruby‘s symbol table.
  #
  #  Symbol.all_symbols.size    #=> 903
  #  Symbol.all_symbols[1,20]   #=> [:floor, :ARGV, :Binding, :symlink,
  #                                  :chown, :EOFError, :$;, :String,
  #                                  :LOCK_SH, :"setuid?", :$<,
  #                                  :default_proc, :compact, :extend,
  #                                  :Tms, :getwd, :$=, :ThreadGroup,
  #                                  :wait2, :$>]
  def self.all_symbols
    # The find_all prunes nil out of the Tuple
    Symbols.symbols.find_all { |s| s }.map { |x| x.to_sym }
  end

  ##
  # Returns a pretty version of the symbol, fit for viewing
  #  :symbol.inspect #=> ":symbol"
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
  
  ##
  # In general, to_sym returns the Symbol corresponding
  # to an object. As sym is already a symbol, self is returned in this case.
  def to_sym
    self
  end

  alias_method :intern, :to_sym

  ##
  # Returns the name or string corresponding to sym.
  #  :fred.id2name   #=> "fred"
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

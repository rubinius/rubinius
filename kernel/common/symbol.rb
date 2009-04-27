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
    Ruby.primitive :symbol_all_symbols
    raise PrimitiveFailure, "Symbol.all_symbols failed."
  end

  ##
  # Returns a pretty version of the symbol, fit for viewing
  #  :symbol.inspect #=> ":symbol"
  def inspect
    str = self.to_s

    case str
    when /^(\$|@@?)[a-z_][a-z_\d]*$/i,                     # Variable names
         /^[a-z_][a-z_\d]*[=?!]?$/i,                        # Method names
         /^\$(-[a-z_\d]|[+~:?<_\/'"$.,`!;\\=*>&@]|\d+)$/i, # Special global variables
         /^([|^&\/%~`]|<<|>>|<=>|===?|=~|[<>]=?|[+-]@?|\*\*?|\[\]=?)$/ # Operators
      ":#{str}"
    else
      ":#{str.inspect}"
    end
  end

  alias_method :intern, :to_sym
  alias_method :id2name, :to_s

end

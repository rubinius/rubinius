class Symbol
  ##
  # Returns a pretty version of the symbol, fit for viewing
  #  :symbol.inspect #=> ":symbol"
  def inspect
    str = to_s

    case str
    when /^(\$|@@?)[a-z_][a-z_\d]*$/i,                      # Variable names
         /^[a-z_][a-z_\d]*[=?!]?$/i,                        # Method names
         /^\$(-[a-z_\d]|[+~:?<_\/'"$.,`!;\\=*>&@]|\d+)$/i,  # Special global variables
         /^([|^&\/%~`]|<<|>>|<=>|===?|=~|[<>]=?|[+-]@?|\*\*?|\[\]=?)$/ # Operators
      ":#{str}"
    else
      ":#{str.inspect}"
    end
  end

  alias_method :intern, :to_sym
  alias_method :id2name, :to_s
end

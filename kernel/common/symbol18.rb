# -*- encoding: us-ascii -*-

class Symbol
  def inspect
    str = to_s

    case str
    when /\A(\$|@@?)[a-z_][a-z_\d]*\z/i,                      # Variable names
         /\A[a-z_][a-z_\d]*[=?!]?\z/i,                        # Method names
         /\A\$(-[a-z_\d]|[+~:?<_\/'"$.,`!;\\=*>&@]|\d+)\z/i,  # Special global variables
         /\A([|^&\/%~`]|<<|>>|<=>|===?|=~|[<>]=?|[+-]@?|\*\*?|\[\]=?)\z/ # Operators
      ":#{str}"
    else
      ":#{str.inspect}"
    end
  end

  alias_method :intern, :to_sym
  alias_method :id2name, :to_s
end

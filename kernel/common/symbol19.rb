# -*- encoding: us-ascii -*-

class Symbol
  def <=>(other)
    return unless other.kind_of? Symbol

    to_s <=> other.to_s
  end

  def capitalize
    to_s.capitalize.to_sym
  end

  def casecmp(other)
    return unless other.kind_of? Symbol

    to_s.casecmp(other.to_s)
  end

  def downcase
    to_s.downcase.to_sym
  end

  ##
  # Returns a pretty version of the symbol, fit for viewing
  #  :symbol.inspect #=> ":symbol"
  def inspect
    str = to_s

    case str
    when /^(\$|@@?)[a-z_][a-z_\d]*$/i,                      # Variable names
         /^[a-z_][a-z_\d]*[=?!]?$/i,                        # Method names
         /^\$(-[a-z_\d]|[+~:?<_\/'"$.,`!;\\=*>&@]|\d+)$/i,  # Special global variables
         /^([|^&\/%~`!]|!=|!~|<<|>>|<=>|===?|=~|[<>]=?|[+-]@?|\*\*?|\[\]=?)$/ # Operators
      ":#{str}"
    else
      ":#{str.inspect}"
    end
  end

  def empty?
    to_s.empty?
  end

  alias_method :intern, :to_sym
  alias_method :id2name, :to_s

  def match(other)
    to_s =~ other
  end

  alias_method :=~, :match

  def swapcase
    to_s.swapcase.to_sym
  end

  def upcase
    to_s.upcase.to_sym
  end

  def succ
    to_s.succ.to_sym
  end

  alias_method :next, :succ

  def [](index, other = undefined)
    if index.kind_of?(Regexp)
      if !other.equal?(undefined)
        match, str = to_s.send(:subpattern, index, other)
        Regexp.last_match = match
        return str
      end

      str = to_s
      match_data = index.search_region(str, 0, str.num_bytes, true)
      Regexp.last_match = match_data
      if match_data
        result = match_data.to_s
        result.taint if index.tainted?
        return result
      end
    else
      to_s[index, other]
    end
  end

  alias_method :slice, :[]
end

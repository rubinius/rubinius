# depends on: class.rb

class Symbol
  def self.all_symbols
    Ruby.primitive :symbol_all_symbols
    raise PrimitiveFailure, "Symbol.all_symbols failed."
  end

  def inspect
    str = to_s
    case str
    when /^\"$/
      ":\"\\\"\""
    when /^(!|!=|!~|&&|'|,|\.|\.\.|\.\.\.|:|::|;|=>|\?|\|\||\d|\+\+)/, /^(\$|@|=)$/, / /
      ":\"#{str}\""
    when /^(-@|\+@|%|&|\*|\*\*|\/|<|<=|<=>|==|===|=~|>|>=|\[\]|\[\]=|<<|^|`|~)/
      ":#{str}"
    when /^($-?|@@?)?[a-z_]+[?!]?/ then # TODO: verify this is correct
      ":#{str}"
    else
      ":\"#{str}\""
    end
  end

  alias_method :intern, :to_sym
  alias_method :id2name, :to_s

  # Slightly modified from ActiveSupport to avoid a reliance on left-to-right
  # evaluation order
  def to_proc
    Proc.new { |rec, *args| rec.__send__(self, *args) }
  end
end

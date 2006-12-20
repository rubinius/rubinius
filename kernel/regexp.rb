class Regexp
  class << self
    def new(str, opts=false)
      create(str, opts)
    end

    def create(str, opts)
      Ruby.primitive :regexp_new
    end

    # FIXME - Optimize me using String#[], String#chr, etc.
    # Do away with the control-character comparisons.
    def escape(str)
      meta = %w![ ] { } ( ) | - * . \\ ? + ^ $ #!
      quoted = ""
      str.codepoints.each do |c|
        quoted << if meta.include?(c)
        "\\#{c}"
        elsif c == "\n"
        "\\n"
        elsif c == "\r"
        "\\r"
        elsif c == "\f"
        "\\f"
        else
          c
        end
      end
      quoted
    end
    alias_method :quote, :escape
  end

  def inspect
    "#<Regexp /#{self.source}/>"
  end

  def match(str)
    Ruby.primitive :regexp_match
  end
end

class MatchData
  def captures
    out = []
    @region.each do |tup|
      x = tup.at(0)
      y = tup.at(1)
      out << @source[x..y]
    end
    return out
  end

  def [](idx)
    if idx == 0
      x = full.at(0)
      y = full.at(1)
      @source[x..y]
    else
      captures[idx - 1]
    end
  end
end

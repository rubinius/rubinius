class Regexp
  def self.new(str, opts=false)
    create(str, opts)
  end
  
  def self.create(str, opts)
    Ruby.primitive :regexp_new
  end
  
  def inspect
    "#<Regexp /#{self.source}/>"
  end
  
  def match(str)
    Ruby.primitive :regexp_match
  end

  # FIXME - Optimize me using String#[], String#chr, etc.
  # Do away with the control-character comparisons.
  def self.escape(str)
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
    self.new(quoted)
  end
end

class MatchData
  def captures
    out = []
    @region.each do |tup|
      x = tup.at(0)
      y = tup.at(1)
      count = y - x + 1
      out << @source.substring(x, count)
    end
    
    return out
  end
end

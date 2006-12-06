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
end

class MatchData
  def captures
    str = self.source
    out = []
    self.region.each do |tup|
      x = tup.at(0)
      y = tup.at(1)
      count = y - x + 1
      out << str.substring(x, count)
    end
    
    return out
  end
end
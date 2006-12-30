class NilClass
  def to_s
    "nil"
  end
  
  alias :inspect :to_s
  
  def nil?
    true
  end

  def to_i
    0
  end
end

class UndefClass
  def undef?
    true
  end
  
  def to_s
    "undef"
  end
end

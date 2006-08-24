class NilClass
  def to_s
    "nil"
  end
  
  def nil?
    true
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
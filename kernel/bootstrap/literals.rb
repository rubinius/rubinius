class NilClass
  def &(other)
    false
  end
  
  def ^(other)
    (other.nil? or other.equal?(false)) ? false : true
  end
    
  def to_s
    ""
  end
  
  def inspect
    "nil"
  end
  
  def nil?
    true
  end
  
  def to_a
    []
  end
  
  def to_f
    0.0
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

TRUE = true
FALSE = false

class TrueClass
  def &(other)
    (other.nil? or other.equal?(false)) ? false : true
  end
  
  def ^(other)
    (other.nil? or other.equal?(false)) ? true : false
  end
  
  def |(other)
    true
  end
  
  def to_s
    "true"
  end
  
end

class FalseClass
  def &(other)
    false
  end
  
  def ^(other)
    (other.equal?(false) or other.nil?) ? false : true
  end
    
  def to_s
    "false"
  end
end

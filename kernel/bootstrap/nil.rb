class NilClass
  def &(other)
    false
  end
  
  def ^(other)
    (other == false or other.nil?) ? false : true
  end
  
  alias :| :^
  
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
    # FIXME: couldn't handle float literals
    '0.0'.to_f
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

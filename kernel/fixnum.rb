class Fixnum
  def ==(o)
    Ruby.primitive :equal
  end
  
  def +(o)
    Ruby.primitive :add
  end
  
  def -(o)
    Ruby.primitive :sub
  end
  
  def *(o)
    Ruby.primitive :fixnum_mul
  end
  
  def <=>(o)
    Ruby.primitive :compare
  end
  
  def <(o)
    (self <=> o) == -1
  end
  
  def <=(o)
    comp = (self <=> o)
    return true if comp == 0 or comp == -1
    return false
  end
  
  def >(o)
    (self <=> o) == 1
  end
  
  def >=(o)
    comp = (self <=> o)
    return true if comp == 0 or comp == 1
    return false
  end
  
  def to_s(base=10)
    based_to_s(base)
  end
  
  def based_to_s(base)
    Ruby.primitive :fixnum_to_s
  end
end
class Fixnum < Integer
  
  alias :modulo :%
  
  def <<(c)
    c = guard_integer_type(c)
    return self >> -c if c < 0

    bits = self.size * 8 - 1
    if (c > bits || self >> bits - c > 0 || Bignum === c)
      return __bignum_new__(self) << c
    end
    __fixnum_left_shift__(c)
  end

  def >>(c)
    c = guard_integer_type(c)
    return self << -c if c < 0
    
    if Bignum === c
      return __bignum_new__(self) >> c
    end

    __fixnum_right_shift__(c)
  end
  
  def id2name
    Symbols.symbols.to_a[self]
  end

  def to_sym
    id2name.to_sym
  rescue NoMethodError
    nil
  end

  # Matches MRI core lib behaviour
  def guard_integer_type(obj)
    return obj if Integer === obj
    obj = obj.to_int
    if Integer === obj
      obj
    else
      raise TypeError, "#{obj.class}#to_i should return Integer"
    end
  rescue NoMethodError
    raise TypeError, "can't convert #{obj.class} into Integer"
  end
  
end


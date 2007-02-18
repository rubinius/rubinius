class Fixnum < Integer
  def %(o)
    Ruby.primitive :fixnum_modulo
    super(o)
  end
  
  alias :modulo :%
  
  def &(o)
    Ruby.primitive :fixnum_and
    super(o)
  end

  def |(o)
    Ruby.primitive :fixnum_or
    super(o)
  end

  def ^(o)
    Ruby.primitive :fixnum_xor
    super(o)
  end

  def ~
    Ruby.primitive :fixnum_invert
  end

  def -@
    Ruby.primitive :fixnum_neg
  end

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

  def __fixnum_left_shift__(c)
    Ruby.primitive :fixnum_left_shift
  end

  def __fixnum_right_shift__(c)
    Ruby.primitive :fixnum_right_shift
  end

  def __bignum_new__(value)
    Ruby.primitive :bignum_new
  end
  
  def to_f
    Ruby.primitive :fixnum_to_f
  end
  
  def divmod(other)
    Ruby.primitive :fixnum_divmod
    super(other)
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


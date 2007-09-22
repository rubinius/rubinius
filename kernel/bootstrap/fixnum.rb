class Fixnum < Integer
  def ==(o)
    Ruby.primitive :equal
    super(o)
  end
  
  def +(o)
    Ruby.primitive :add
    super(o)
  end
  
  def -(o)
    Ruby.primitive :sub
    super(o)
  end
  
  def *(o)
    Ruby.primitive :fixnum_mul
    super(o)
  end
  
  def /(o)
    Ruby.primitive :fixnum_div
    super(o)
  end
  
  def %(o)
    Ruby.primitive :fixnum_modulo
    super(o)
  end
  
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
  
  def divmod(other)
    Ruby.primitive :fixnum_divmod
    super(other)
  end
  
  
  
  def <=>(other)
    Ruby.primitive :compare
    super(other)
  end

  def <(o)
    (self <=> o) == -1
  end
  
  def <=(o)
    (self <=> o) != 1
  end
  
  def >(o)
    (self <=> o) == 1
  end
  
  def >=(o)
    (self <=> o) != -1
  end
  
  def to_s(base=10)
    based_to_s(base)
  end
  
  def based_to_s(base)
    Ruby.primitive :fixnum_to_s
  end
  
  def to_f
    Ruby.primitive :fixnum_to_f
  end
  
  def size
    Ruby.primitive :fixnum_size
  end

  def inspect
    based_to_s(10)
  end

  def zero?
    self == 0
  end

  def nonzero?
    self != 0
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
end

class Numeric
  def coerce(other)
    Ruby.primitive(:numeric_coerce) # try to keep bignum/fixnum precision
    [Float(other), Float(self)]
  end
end

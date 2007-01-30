class Fixnum < Integer
  def ==(o)
    Ruby.primitive :equal
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
  end
  
  def &(o)
    Ruby.primitive :fixnum_and
  end

  def |(o)
    Ruby.primitive :fixnum_or
  end

  def ^(o)
    Ruby.primitive :fixnum_xor
  end

  def ~
    Ruby.primitive :fixnum_invert
  end

  def -@
    Ruby.primitive :fixnum_neg
  end

  def <<(c)
    self.__fixnum_shift__(c, -1)
  end

  def >>(c)
    self.__fixnum_shift__(c, 1)
  end

  def __fixnum_shift__(count, direction)
    Ruby.primitive :fixnum_shift
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

  def <=>(other)
    Ruby.primitive :compare
    super(other)
  end
  
  def to_f
    Ruby.primitive :fixnum_to_f
  end
end

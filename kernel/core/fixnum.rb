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
    self.__fixnum_shift__(c, -1)
  end

  def >>(c)
    self.__fixnum_shift__(c, 1)
  end

  def __fixnum_shift__(count, direction)
    Ruby.primitive :fixnum_shift
  end

  def to_f
    Ruby.primitive :fixnum_to_f
  end
  
  def divmod(other)
    Ruby.primitive :fixnum_divmod
    super(other)
  end
  
  def div(other)
    Integer === other ? self / other : self / Integer(other)
  end
end

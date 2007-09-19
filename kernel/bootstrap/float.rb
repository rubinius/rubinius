class Float
  def to_s
    Ruby.primitive :float_to_s
  end
  
  def to_s_formatted(format)
    Ruby.primitive :float_sprintf
  end    
  
  def +(other)
    Ruby.primitive :float_add
    super(other)
  end

  def -(other)
    Ruby.primitive :float_sub
    super(other)
  end

  def *(other)
    Ruby.primitive :float_mul
    super(other)
  end

  def /(other)
    Ruby.primitive :float_div
    super(other)
  end
  
  def ==(o)
    Ruby.primitive :float_equal
    super(o)
  end

  def eql?(other)
    Ruby.primitive :float_equal
    false
  end
  
  def <=>(other)
    Ruby.primitive :float_compare
    super(other)
  end

  def nan?
    Ruby.primitive :float_nan_p
  end

  def infinite?
    Ruby.primitive :float_infinite_p
  end
  
  def -@
    Ruby.primitive :float_uminus
  end
  
  def **(other)
    Ruby.primitive :float_pow
    super(other)
  end
  
  # def to_i
  #   Ruby.primitive :float_to_i
  #   raise FloatDomainError, "Infinity" if infinite?
  #   self if nan?
  # end
  
  def to_f
    self
  end
  
  def divmod(other)
    Ruby.primitive :float_divmod
    raise FloatDomainError, "divide by 0" if other == 0
    super(other)
  end
  
  def round
    Ruby.primitive :float_round
  end
end
class Float
  def to_s
    Ruby.primitive :float_to_s
  end
  
  def to_s_formatted(format)
    Ruby.primitive :float_sprintf
  end    
  
  # def ==(o)
  #   Ruby.primitive :float_equal
  #   super(o)
  # end
  # 
  # def eql?(other)
  #   Ruby.primitive :float_equal
  #   false
  # end
  # 
  # def <=>(other)
  #   Ruby.primitive :float_compare
  #   super(other)
  # end

  def nan?
    Ruby.primitive :float_nan_p
  end

  def infinite?
    Ruby.primitive :float_infinite_p
  end
  
  def **(other)
    Ruby.primitive :float_pow
    super(other)
  end
  
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
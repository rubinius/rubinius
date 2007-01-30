class Float < Numeric
  def nan?
    Ruby.primitive :float_nan_p
  end

  def infinite?
    Ruby.primitive :float_infinite_p
  end

  def finite?
    infinite?.nil?
  end

  def to_f
    self
  end
  
  def -@
    Ruby.primitive :float_uminus
  end
  
  def **(other)
    Ruby.primitive :float_pow
  end
  
  def to_i
    Ruby.primitive :float_to_i
  end
  
  alias :to_int :to_i
  alias :truncate :to_i
end


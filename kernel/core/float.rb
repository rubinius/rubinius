class Float
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
  
  def **(o)
    Ruby.primitive :float_pow
  end
end

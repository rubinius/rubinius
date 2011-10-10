class NilClass
  def to_r
    Rational(0,1)
  end
  
  def rationalize(*args)
    to_r
  end
end
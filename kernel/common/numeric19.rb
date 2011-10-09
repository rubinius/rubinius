class Numeric
  def i
    Complex(0, self)
  end
  
  def rationalize
    Rational(self,1)
  end
  
  def rect
    [self, 0]
  end
  alias_method :rectangular, :rect

end

class Numeric
  def i
    Complex(0, self)
  end

  def imag
    0
  end
  alias_method :imaginary, :imag
  
  def rationalize
    Rational(self,1)
  end
  
  def rect
    [self, 0]
  end
  alias_method :rectangular, :rect

  def abs2
    self * self
  end

  alias_method :magnitude, :abs
end

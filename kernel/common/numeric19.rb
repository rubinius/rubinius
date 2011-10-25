class Numeric
  def i
    Complex(0, self)
  end

  def to_c
    Complex(self, 0)
  end

  def imag
    0
  end
  alias_method :imaginary, :imag

  def rationalize(eps = nil)
    Rational(self, 1)
  end

  def rect
    [self, 0]
  end
  alias_method :rectangular, :rect

  def abs2
    self * self
  end

  alias_method :magnitude, :abs

  def numerator
    to_r.numerator
  end

  def denominator
    to_r.denominator
  end
end

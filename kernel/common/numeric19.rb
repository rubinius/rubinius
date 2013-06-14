# -*- encoding: us-ascii -*-

class Numeric
  def div(other)
    raise ZeroDivisionError, "divided by 0" if other == 0
    self.__slash__(other).floor
  end

  def fdiv(other)
    self.to_f / other
  end

  def quo(other)
    Rubinius.privately do
      Rational.convert(self, 1, false) / other
    end
  end

  def modulo(other)
    self - other * self.div(other)
  end
  alias_method :%, :modulo

  def i
    Complex(0, self)
  end

  def to_c
    Complex(self, 0)
  end

  def real
    self
  end

  def imag
    0
  end
  alias_method :imaginary, :imag

  def arg
    if self < 0
      Math::PI
    else
      0
    end
  end
  alias_method :angle, :arg
  alias_method :phase, :arg

  def polar
    return abs, arg
  end

  def conjugate
    self
  end
  alias_method :conj, :conjugate

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

  def real?
    true
  end
end

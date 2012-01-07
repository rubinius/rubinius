# -*- encoding: us-ascii -*-

class Fixnum < Integer
  def imaginary
    0
  end

  # Must be it's own method, so that super calls the correct method
  # on Numeric
  def div(o)
    Rubinius.primitive :fixnum_div

    if o.is_a?(Float) && o == 0.0
      raise ZeroDivisionError, "division by zero"
    end

    redo_coerced :div, o
  end

  def **(o)
    Rubinius.primitive :fixnum_pow

    if o.is_a?(Float) && self < 0 && o != o.round
      return Complex(self) ** o
    elsif o.is_a?(Integer) && o < 0
      return Rational(self) ** o
    end

    redo_coerced :**, o
  end
end

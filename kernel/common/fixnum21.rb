# -*- encoding: us-ascii -*-

class Fixnum < Integer
  def imaginary
    0
  end

  # Must be it's own method, so that super calls the correct method
  # on Numeric
  def div(o)
    if o.is_a?(Float) && o == 0.0
      raise ZeroDivisionError, "division by zero"
    end
    divide(o).floor
  end

  def **(o)
    Rubinius.primitive :fixnum_pow

    if o.is_a?(Float) && self < 0 && o != o.round
      return Complex.new(self, 0) ** o
    elsif o.is_a?(Integer) && o < 0
      return Rational.new(self, 1) ** o
    end

    redo_coerced :**, o
  end
end

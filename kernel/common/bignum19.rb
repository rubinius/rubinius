# -*- encoding: us-ascii -*-

class Bignum < Integer
  def **(o)
    Rubinius.primitive :bignum_pow

    if o.is_a?(Float) && self < 0 && o != o.round
      return Complex(self) ** o
    elsif o.is_a?(Integer) && o < 0
      return Rational(self) ** o
    end

    redo_coerced :**, o
  end
end

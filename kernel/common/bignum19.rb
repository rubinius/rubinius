class Bignum < Integer
  def **(o)
    Rubinius.primitive :bignum_pow

    if o.is_a?(Float) && self < 0 && o != o.round
      return Complex(self) ** o
    end

    redo_coerced :**, o
  end
end

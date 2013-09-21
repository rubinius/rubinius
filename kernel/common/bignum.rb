class Bignum < Integer

  # see README-DEVELOPERS regarding safe math compiler plugin
  alias_method :/, :divide

  def eql?(value)
    value.is_a?(Bignum) && self == value
  end

  alias_method :modulo, :%

  def fdiv(n)
    to_f / n
  end

  def **(o)
    Rubinius.primitive :bignum_pow

    if o.is_a?(Float) && self < 0 && o != o.round
      return Complex.new(self, 0) ** o
    elsif o.is_a?(Integer) && o < 0
      return Rational.new(self, 1) ** o
    end

    redo_coerced :**, o
  end
end

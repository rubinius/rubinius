class Float
  INFINITY = 1.0 / 0.0
  NAN = 0.0 / 0.0

  def **(other)
    Rubinius.primitive :float_pow

    if other.is_a?(Float) && self < 0 && other != other.round
      return Complex(self) ** other
    end

    b, a = math_coerce other
    a ** b
  end

  alias_method :magnitude, :abs

  def imaginary
    0
  end

  def numerator
    if nan?
      NAN
    elsif infinite? == 1
      INFINITY
    elsif infinite? == -1
      -INFINITY
    else
      super
    end
  end

  def denominator
    if infinite? || nan?
      1
    else
      super
    end
  end

  def to_r
    f, e = Math.frexp self
    f = Math.ldexp(f, MANT_DIG).to_i
    e -= MANT_DIG

    (f * (RADIX ** e)).to_r
  end

  def arg
    return self if nan?
    super
  end
  alias_method :angle, :arg
  alias_method :phase, :arg

  def rationalize(eps=undefined)
    if eps.equal?(undefined)
      f, n = Math.frexp self
      f = Math.ldexp(f, Float::MANT_DIG).to_i
      n -= Float::MANT_DIG

      Rational(2 * f, 1 << (1 - n)).rationalize(Rational(1, 1 << (1 - n)))
    else
      to_r.rationalize(eps)
    end
  end

  def round(precision=0)
    precision = Rubinius::Type.coerce_to(precision, Integer, :to_int)

    raise FloatDomainError if precision < 1 && infinite?
    raise RangeError if precision < 1 && nan?
    return self if precision > 0 && (infinite? || nan?)

    Rubinius.primitive :float_round
    raise PrimitiveFailure, "float_round failed"
  end
end

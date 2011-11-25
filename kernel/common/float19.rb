class Float
  INFINITY = 1.0 / 0.0
  NAN = 0.0 / 0.0

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
end

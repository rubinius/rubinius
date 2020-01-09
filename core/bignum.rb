class Bignum < Integer
  # unary operators

  # binary math operators

  def bit_length
    Rubinius.invoke_primitive :bignum_bit_length, self > 0 ? self : ~self
  end

  # This is separate from divide because it calls a different method
  # if it has to coerce. This is important because Bignum#div(Float) and
  # Bignum#/(Float) return different things.
  #
  def div(other)
    Rubinius.primitive :bignum_div
    redo_coerced :div, other
  end

  def divmod(other)
    Rubinius.primitive :bignum_divmod
    redo_coerced :divmod, other
  end

  # comparison operators

  def <=>(other)
    Rubinius.primitive :bignum_compare

    # We do not use redo_compare here because Ruby does not
    # raise if any part of the coercion or comparison raises
    # an exception.
    begin
      coerced = other.coerce self
      return nil unless coerced
      coerced[0] <=> coerced[1]
    rescue
      return nil
    end
  end

  # conversions

  def coerce(other)
    Rubinius.primitive :bignum_coerce
    super other
  end

  def to_f
    Rubinius.primitive :bignum_to_float
    raise PrimitiveFailure, "Bignum#to_f primitive failed"
  end

  def to_s(base=10)
    Rubinius.invoke_primitive :bignum_to_s, self, base
  end

  # We do not alias this to #to_s in case someone overrides #to_s.
  def inspect
    Rubinius.invoke_primitive :bignum_to_s, self, 10
  end

  def eql?(value)
    value.is_a?(Bignum) && self == value
  end

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

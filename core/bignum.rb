class Bignum < Integer
  # unary operators

  # binary math operators

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

  def eql?(value)
    value.is_a?(Bignum) && self == value
  end

  def fdiv(n)
    to_f / n
  end
end

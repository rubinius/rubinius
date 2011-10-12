class Bignum < Integer
  def divmod(other)
    Rubinius.primitive :bignum_divmod

    if other == 0.0 && other.is_a?(Float)
      raise ZeroDivisionError, "divided by 0"
    end

    redo_coerced :divmod, other
  end
end

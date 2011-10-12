class Bignum < Integer
  def divmod(other)
    Rubinius.primitive :bignum_divmod
    redo_coerced :divmod, other
  end
end

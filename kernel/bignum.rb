class Bignum < Integer
  def radix_to_s(radix)
    Ruby.primitive :bignum_to_s
  end

  def to_s(radix=10)
    radix_to_s(radix)
  end

  def inspect
    radix_to_s(10)
  end
  
  def +(o)
    Ruby.primitive :bignum_add
  end

  def -(o)
    Ruby.primitive :bignum_sub
  end

  def *(o)
    Ruby.primitive :bignum_mul
  end

  def ==(o)
    Ruby.primitive :bignum_equal
  end
end

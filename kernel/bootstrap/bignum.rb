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
    super(o)
  end

  def -(o)
    Ruby.primitive :bignum_sub
    super(o)
  end

  def *(o)
    Ruby.primitive :bignum_mul
    super(o)
  end
  
  def /(o)
    Ruby.primitive :bignum_div
    super(o)
  end
  
  def &(o)
    Ruby.primitive :bignum_and
  end

  def |(o)
    Ruby.primitive :bignum_or
  end

  def ^(o)
    Ruby.primitive :bignum_xor
  end

  def ~
    Ruby.primitive :bignum_invert
  end

  def -@
    Ruby.primitive :bignum_neg
  end

  def ==(o)
    Ruby.primitive :bignum_equal
  end

  def to_f
    Ruby.primitive :bignum_to_float
  end

  def <=>(other)
    Ruby.primitive :bignum_compare
    super(other)
  end
end


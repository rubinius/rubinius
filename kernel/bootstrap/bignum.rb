class Bignum < Integer
  def self.from_float(value)
    Ruby.primitive :bignum_from_float
  end
  
  def radix_to_s(radix)
    Ruby.primitive :bignum_to_s
  end

  def to_s(radix=10)
    raise ArgumentError, 'base must be between 2 and 36' unless
      radix.between?(2, 36)
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
    super(o)
  end

  def |(o)
    Ruby.primitive :bignum_or
    super(o)
  end

  def ^(o)
    Ruby.primitive :bignum_xor
    super(o)
  end

  def ~
    Ruby.primitive :bignum_invert
  end

  def -@
    Ruby.primitive :bignum_neg
  end

  def ==(o)
    Ruby.primitive :bignum_equal
    super(o)
  end
  
  def __bignum_left_shift__(s)
    Ruby.primitive :bignum_left_shift
  end

  def __bignum_right_shift__(s)
    Ruby.primitive :bignum_right_shift
  end

  def to_f
    Ruby.primitive :bignum_to_float
  end

  def <=>(other)
    Ruby.primitive :bignum_compare
    super(other)
  end
  
  def divmod(other)
    Ruby.primitive :bignum_divmod
    super(other)
  end

  def mod_primitive(other)
    Ruby.primitive :bignum_mod
  end

  def size
    Ruby.primitive :bignum_size
  end
end

class Bignum < Integer

  # unary operators

  def ~
    Ruby.primitive :bignum_invert
    raise PrimitiveFailure, "Bignum#~ primitive failed"
  end

  def -@
    Ruby.primitive :bignum_neg
    raise PrimitiveFailure, "Bignum#-@ primitive failed"
  end

  # binary math operators

  def +(o)
    Ruby.primitive :bignum_add
    redo_coerced :+, o
  end

  def -(o)
    Ruby.primitive :bignum_sub
    redo_coerced :-, o
  end

  def *(o)
    Ruby.primitive :bignum_mul
    redo_coerced :*, o
  end

  # this method is aliased to / in core
  # see README-DEVELOPERS regarding safe math compiler plugin
  def divide(o)
    Ruby.primitive :bignum_div
    redo_coerced :/, o
  end

  # This is seperate from divide because it calls a different method
  # if it has to coerce. This is important because Bignum#div(Float) and
  # Bignum#/(Float) return different things.
  #
  def div(other)
    Ruby.primitive :bignum_div
    redo_coerced :div, other
  end

  def %(other)
    Ruby.primitive :bignum_mod
    redo_coerced :%, other
  end

  def divmod(other)
    Ruby.primitive :bignum_divmod
    redo_coerced :divmod, other
  end

  # bitwise binary operators

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

  def <<(other)
    Ruby.primitive :bignum_left_shift

    other = Type.coerce_to other, Integer, :to_int
    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self << other
  end

  def >>(other)
    Ruby.primitive :bignum_right_shift

    other = Type.coerce_to other, Integer, :to_int
    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self >> other
  end

  def **(o)
    Ruby.primitive :bignum_pow
    redo_coerced :**, o
  end

  # comparison operators

  def <(other)
    Ruby.primitive :bignum_lt
    b, a = math_coerce other, :compare_error
    a < b
  end

  def <=(other)
    Ruby.primitive :bignum_le
    b, a = math_coerce other, :compare_error
    a <= b
  end

  def >(other)
    Ruby.primitive :bignum_gt
    b, a = math_coerce other, :compare_error
    a > b
  end

  def >=(other)
    Ruby.primitive :bignum_ge
    b, a = math_coerce other, :compare_error
    a >= b
  end

  def ==(o)
    Ruby.primitive :bignum_equal
    # This is to make sure the return value is true or false
    return true if o == self
    false
  end

  def <=>(other)
    Ruby.primitive :bignum_compare
    super(other)
  end

  # conversions

  def self.from_float(value)
    Ruby.primitive :bignum_from_float
    raise PrimitiveFailure, "Bignum.from_float primitive failed"
  end

  def coerce(other)
    Ruby.primitive :bignum_coerce
    super other
  end

  def to_f
    Ruby.primitive :bignum_to_float
    raise PrimitiveFailure, "Bignum#to_f primitive failed"
  end

  def to_s(base=10)
    Rubinius.invoke_primitive :bignum_to_s, self, base
  end

  # We do not alias this to #to_s in case someone overrides #to_s.
  def inspect
    Rubinius.invoke_primitive :bignum_to_s, self, 10
  end

  def size
    Ruby.primitive :bignum_size
    raise PrimitiveFailure, "Bignum#size primitive failed"
  end
end

class Fixnum < Integer

  def self.===(obj)
    Ruby.primitive :fixnum_s_eqq
    super
  end

  # unary operators

  def ~
    Ruby.primitive :fixnum_invert
    raise PrimitiveFailure, "Fixnum#~ primitive failed"
  end

  def -@
    Ruby.primitive :fixnum_neg
    raise PrimitiveFailure, "Fixnum#-@ primitive failed"
  end

  # binary math operators

  def +(o)
    Ruby.primitive :fixnum_add
    redo_coerced :+, o
  end

  def -(o)
    Ruby.primitive :fixnum_sub
    redo_coerced :-, o
  end

  def *(o)
    Ruby.primitive :fixnum_mul
    redo_coerced :*, o
  end

  # this method is aliased to / in core
  # see README-DEVELOPERS regarding safe math compiler plugin
  def divide(o)
    Ruby.primitive :fixnum_div
    redo_coerced :/, o
  end
  alias_method :/, :divide

  # Must be it's own method, so that super calls the correct method
  # on Numeric
  def div(o)
    Ruby.primitive :fixnum_div
    redo_coerced :div, o
  end

  def %(o)
    Ruby.primitive :fixnum_mod
    redo_coerced :%, o
  end

  def divmod(other)
    Ruby.primitive :fixnum_divmod
    redo_coerced :divmod, other
  end

  # bitwise binary operators

  def &(o)
    Ruby.primitive :fixnum_and
    if o.__kind_of__ Float
      raise RangeError, ("float %-.10g is out of range of a Fixnum" % o)
    end
    super(o)
  end

  def |(o)
    Ruby.primitive :fixnum_or
    if o.__kind_of__ Float
      raise RangeError, ("float %-.10g is out of range of a Fixnum" % o)
    end
    super(o)
  end

  def ^(o)
    Ruby.primitive :fixnum_xor
    if o.__kind_of__ Float
      raise RangeError, ("float %-.10g is out of range of a Fixnum" % o)
    end
    super(o)
  end

  def <<(o)
    Ruby.primitive :fixnum_left_shift
    super(o)
  end

  def >>(o)
    Ruby.primitive :fixnum_right_shift
    super(o)
  end

  def **(o)
    Ruby.primitive :fixnum_pow
    redo_coerced :**, o
  end

  def __bignum_new__(value)
    Ruby.primitive :bignum_new
    raise PrimitiveFailure, "primitive failed"
  end

  # comparison operators

  def ==(o)
    Ruby.primitive :fixnum_equal
    super(o)
  end

  def <=>(other)
    Ruby.primitive :fixnum_compare

    # DO NOT super to Numeric#<=>. It does not contain the coerce
    # protocol.

    begin
      b, a = math_coerce(other, :compare_error)
      return a <=> b
    rescue ArgumentError
      return nil
    end
  end

  def <(other)
    Ruby.primitive :fixnum_lt

    b, a = math_coerce other, :compare_error
    a < b
  end

  def <=(other)
    Ruby.primitive :fixnum_le

    b, a = math_coerce other, :compare_error
    a <= b
  end

  def >(other)
    Ruby.primitive :fixnum_gt

    b, a = math_coerce other, :compare_error
    a > b
  end

  def >=(other)
    Ruby.primitive :fixnum_ge

    b, a = math_coerce other, :compare_error
    a >= b
  end

  # predicates

  def zero?
    self == 0
  end

  # conversions

  def coerce(other)
    Ruby.primitive :fixnum_coerce
    super other
  end

  def to_s(base=10)
    Rubinius.invoke_primitive :fixnum_to_s, self, base
  end

  # We do not alias this to #to_s in case someone overrides #to_s.
  def inspect
    Rubinius.invoke_primitive :fixnum_to_s, self, 10
  end

  def to_f
    Ruby.primitive :fixnum_to_f
    raise PrimitiveFailure, "Fixnum#to_f primitive failed"
  end

  def size
    Ruby.primitive :fixnum_size
    raise PrimitiveFailure, "Fixnum#size primitive failed"
  end
end

# -*- encoding: us-ascii -*-

class Bignum < Integer

  # unary operators

  def ~
    Rubinius.primitive :bignum_invert
    raise PrimitiveFailure, "Bignum#~ primitive failed"
  end

  def -@
    Rubinius.primitive :bignum_neg
    raise PrimitiveFailure, "Bignum#-@ primitive failed"
  end

  # binary math operators

  def +(o)
    Rubinius.primitive :bignum_add
    redo_coerced :+, o
  end

  def -(o)
    Rubinius.primitive :bignum_sub
    redo_coerced :-, o
  end

  def *(o)
    Rubinius.primitive :bignum_mul
    redo_coerced :*, o
  end

  # this method is aliased to / in core
  # see README-DEVELOPERS regarding safe math compiler plugin
  def divide(o)
    Rubinius.primitive :bignum_div
    redo_coerced :/, o
  end

  # This is separate from divide because it calls a different method
  # if it has to coerce. This is important because Bignum#div(Float) and
  # Bignum#/(Float) return different things.
  #
  def div(other)
    Rubinius.primitive :bignum_div
    redo_coerced :div, other
  end

  def %(other)
    Rubinius.primitive :bignum_mod
    redo_coerced :%, other
  end

  def divmod(other)
    Rubinius.primitive :bignum_divmod
    redo_coerced :divmod, other
  end

  # bitwise binary operators

  def &(o)
    Rubinius.primitive :bignum_and
    super(o)
  end

  def |(o)
    Rubinius.primitive :bignum_or
    super(o)
  end

  def ^(o)
    Rubinius.primitive :bignum_xor
    super(o)
  end

  def <<(other)
    Rubinius.primitive :bignum_left_shift

    other = Rubinius::Type.coerce_to other, Integer, :to_int
    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self << other
  end

  def >>(other)
    Rubinius.primitive :bignum_right_shift

    other = Rubinius::Type.coerce_to other, Integer, :to_int
    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self >> other
  end

  # comparison operators

  def <(other)
    Rubinius.primitive :bignum_lt
    redo_compare :<, other
  end

  def <=(other)
    Rubinius.primitive :bignum_le
    redo_compare :<=, other
  end

  def >(other)
    Rubinius.primitive :bignum_gt
    redo_compare :>, other
  end

  def >=(other)
    Rubinius.primitive :bignum_ge
    redo_compare :>=, other
  end

  def ==(o)
    Rubinius.primitive :bignum_equal
    # This is to make sure the return value is true or false
    o == self ? true : false
  end

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

  def self.from_float(value)
    Rubinius.primitive :bignum_from_float
    raise PrimitiveFailure, "Bignum.from_float primitive failed"
  end

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

  def size
    Rubinius.primitive :bignum_size
    raise PrimitiveFailure, "Bignum#size primitive failed"
  end
end

##
#--
# NOTE do not define to_sym or id2name. It's been deprecated for 5 years and
# we've decided to remove it.
#++
class Fixnum < Integer
  def self.===(obj)
    Rubinius.primitive :fixnum_s_eqq
    super
  end

  # unary operators

  def ~
    Rubinius.primitive :fixnum_invert
    raise PrimitiveFailure, "Fixnum#~ primitive failed"
  end

  def -@
    Rubinius.primitive :fixnum_neg
    raise PrimitiveFailure, "Fixnum#-@ primitive failed"
  end

  # binary math operators

  def +(o)
    Rubinius.primitive :fixnum_add
    redo_coerced :+, o
  end

  def -(o)
    Rubinius.primitive :fixnum_sub
    redo_coerced :-, o
  end

  def *(o)
    Rubinius.primitive :fixnum_mul
    redo_coerced :*, o
  end

  # this method is aliased to / in core
  # see README-DEVELOPERS regarding safe math compiler plugin
  def divide(o)
    Rubinius.primitive :fixnum_div
    redo_coerced :/, o
  end
  alias_method :/, :divide

  def %(o)
    Rubinius.primitive :fixnum_mod
    redo_coerced :%, o
  end

  def divmod(other)
    Rubinius.primitive :fixnum_divmod
    redo_coerced :divmod, other
  end

  # bitwise binary operators

  def &(o)
    Rubinius.primitive :fixnum_and
    super(o)
  end

  def |(o)
    Rubinius.primitive :fixnum_or
    super(o)
  end

  def ^(o)
    Rubinius.primitive :fixnum_xor
    super(o)
  end

  def <<(other)
    Rubinius.primitive :fixnum_left_shift

    other = Rubinius::Type.coerce_to other, Integer, :to_int

    self >> -other if other < 0

    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self << other
  end

  def >>(other)
    Rubinius.primitive :fixnum_right_shift

    other = Rubinius::Type.coerce_to other, Integer, :to_int

    self << -other if other < 0

    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self >> other
  end

  # comparison operators

  def ==(o)
    Rubinius.primitive :fixnum_equal
    o == self
  end

  alias_method :===, :==

  def <=>(other)
    Rubinius.primitive :fixnum_compare

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
    Rubinius.primitive :fixnum_lt

    b, a = math_coerce other, :compare_error
    a < b
  end

  def <=(other)
    Rubinius.primitive :fixnum_le

    b, a = math_coerce other, :compare_error
    a <= b
  end

  def >(other)
    Rubinius.primitive :fixnum_gt

    b, a = math_coerce other, :compare_error
    a > b
  end

  def >=(other)
    Rubinius.primitive :fixnum_ge

    b, a = math_coerce other, :compare_error
    a >= b
  end

  # predicates

  def zero?
    self == 0
  end

  # conversions

  def coerce(other)
    Rubinius.primitive :fixnum_coerce
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
    Rubinius.primitive :fixnum_to_f
    raise PrimitiveFailure, "Fixnum#to_f primitive failed"
  end

  def size
    Rubinius.primitive :fixnum_size
    raise PrimitiveFailure, "Fixnum#size primitive failed"
  end

  def self.induced_from(obj)
    case obj
    when Fixnum
      return obj
    when Float, Bignum
      value = obj.to_i
      if value.is_a? Bignum
        raise RangeError, "Object is out of range for a Fixnum"
      else
        return value
      end
    else
      value = Rubinius::Type.coerce_to(obj, Integer, :to_int)
      return self.induced_from(value)
    end
  end

  #--
  # see README-DEVELOPERS regarding safe math compiler plugin
  #++

  alias_method :/, :divide
  alias_method :modulo, :%

  def bit_length
    Math.log2(self < 0 ? -self : self+1).ceil
  end

  def fdiv(n)
    if n.kind_of?(Fixnum)
      to_f / n
    else
      redo_coerced :fdiv, n
    end
  end

  def imaginary
    0
  end

  # Must be it's own method, so that super calls the correct method
  # on Numeric
  def div(o)
    if o.is_a?(Float) && o == 0.0
      raise ZeroDivisionError, "division by zero"
    end
    divide(o).floor
  end

  def **(o)
    Rubinius.primitive :fixnum_pow

    if o.is_a?(Float) && self < 0 && o != o.round
      return Complex.new(self, 0) ** o
    elsif o.is_a?(Integer) && o < 0
      return Rational.new(self, 1) ** o
    end

    redo_coerced :**, o
  end
end

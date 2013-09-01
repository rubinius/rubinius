# -*- encoding: us-ascii -*-

class Float < Numeric

  include Precision

  FFI = Rubinius::FFI

  def self.induced_from(obj)
    case obj
    when Float, Bignum, Fixnum
      obj.to_f
    else
      raise TypeError, "failed to convert #{obj.class} into Float"
    end
  end

  def coerce(other)
    return [other, self] if other.kind_of? Float
    [Float(other), self]
  end

  def -@
    Rubinius.primitive :float_neg
    raise PrimitiveFailure, "Float#-@ primitive failed"
  end

  def abs
    FFI::Platform::Math.fabs(self)
  end

  def negative?
    Rubinius.primitive :float_negative
    raise PrimitiveFailure, "Float#negative primitive failed"
  end

  def +(other)
    Rubinius.primitive :float_add
    b, a = math_coerce other
    a + b
  end

  def -(other)
    Rubinius.primitive :float_sub
    b, a = math_coerce other
    a - b
  end

  def *(other)
    Rubinius.primitive :float_mul
    b, a = math_coerce other
    a * b
  end

  #--
  # see README-DEVELOPERS regarding safe math compiler plugin
  #++

  def divide(other)
    Rubinius.primitive :float_div
    redo_coerced :/, other
  end

  alias_method :/, :divide
  alias_method :quo, :/
  alias_method :fdiv, :/


  def divmod(other)
    Rubinius.primitive :float_divmod
    b, a = math_coerce other
    a.divmod b
  end

  def %(other)
    return 0 / 0.to_f if other == 0
    Rubinius.primitive :float_mod
    b, a = math_coerce other
    a % b
  end

  alias_method :modulo, :%

  def <(other)
    Rubinius.primitive :float_lt
    b, a = math_coerce other, :compare_error
    a < b
  end

  def <=(other)
    Rubinius.primitive :float_le
    b, a = math_coerce other, :compare_error
    a <= b
  end

  def >(other)
    Rubinius.primitive :float_gt
    b, a = math_coerce other, :compare_error
    a > b
  end

  def >=(other)
    Rubinius.primitive :float_ge
    b, a = math_coerce other, :compare_error
    a >= b
  end

  def <=>(other)
    Rubinius.primitive :float_compare
    b, a = math_coerce other, :compare_error
    a <=> b
  rescue ArgumentError
    nil
  end

  def ==(other)
    Rubinius.primitive :float_equal
    begin
      b, a = math_coerce(other)
      return a == b
    rescue TypeError
      return other == self
    end
  end

  def eql?(other)
    Rubinius.primitive :float_eql
    false
  end

  def nan?
    Rubinius.primitive :float_isnan
    raise PrimitiveFailure, "Float#nan? primitive failed"
  end

  def infinite?
    Rubinius.primitive :float_isinf
    raise PrimitiveFailure, "Float#infinite? primitive failed"
  end

  def finite?
    not (nan? or infinite?)
  end

  def to_f
    self
  end

  def to_i
    Rubinius.primitive :float_to_i
    raise PrimitiveFailure, "Float#to_i primitive failed"
  end

  alias_method :to_int, :to_i
  alias_method :truncate, :to_i

  def to_s
    to_s_minimal
  end

  alias_method :inspect, :to_s

  def to_s_minimal
    Rubinius.primitive :float_to_s_minimal
    raise PrimitiveFailure, "Float#to_s_minimal primitive failed: output exceeds buffer size"
  end

  def to_s_formatted(fmt)
    Rubinius.primitive :float_to_s_formatted
    raise PrimitiveFailure, "Float#to_s_formatted primitive failed: output exceeds buffer size"
  end
  private :to_s_formatted

  def dtoa
    Rubinius.primitive :float_dtoa
    raise PrimitiveFailure, "Fload#dtoa primitive failed"
  end

  def to_packed(size)
    Rubinius.primitive :float_to_packed
    raise PrimitiveFailure, "Float#to_packed primitive failed"
  end

  def ceil
    int = to_i()

    return int if self == int or self < 0
    return int + 1
  end

  def floor
    int = to_i()

    return int if self > 0 or self == int
    return int - 1
  end
end


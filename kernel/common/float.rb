class Float < Numeric

  include Precision

  def self.induced_from(obj)
    case obj
    when Float, Bignum, Fixnum
      obj.to_f
    else
      raise TypeError, "failed to convert #{obj.class} into Float"
    end
  end

  alias_method :fdiv, :/

  def coerce(other)
    return [other, self] if other.kind_of? Float
    [Float(other), self]
  end

  def -@
    Ruby.primitive :float_neg
    raise PrimitiveFailure, "Float#-@ primitive failed"
  end

  def +(other)
    Ruby.primitive :float_add
    b, a = math_coerce other
    a + b
  end

  def -(other)
    Ruby.primitive :float_sub
    b, a = math_coerce other
    a - b
  end

  def *(other)
    Ruby.primitive :float_mul
    b, a = math_coerce other
    a * b
  end

  #--
  # see README-DEVELOPERS regarding safe math compiler plugin
  #++

  def divide(other)
    Ruby.primitive :float_div
    b, a = math_coerce other
    a.divide b
  end
  alias_method :/, :divide
  alias_method :quo, :/

  def divmod(other)
    Ruby.primitive :float_divmod
    b, a = math_coerce other
    a.divmod b
  end

  def **(other)
    Ruby.primitive :float_pow
    b, a = math_coerce other
    a ** b
  end

  def %(other)
    return 0 / 0.to_f if other == 0
    Ruby.primitive :float_mod
    b, a = math_coerce other
    a % b
  end
  alias_method :modulo, :%

  def <(other)
    Ruby.primitive :float_lt
    b, a = math_coerce other, :compare_error
    a < b
  end

  def <=(other)
    Ruby.primitive :float_le
    b, a = math_coerce other, :compare_error
    a <= b
  end

  def >(other)
    Ruby.primitive :float_gt
    b, a = math_coerce other, :compare_error
    a > b
  end

  def >=(other)
    Ruby.primitive :float_ge
    b, a = math_coerce other, :compare_error
    a >= b
  end

  def <=>(other)
    Ruby.primitive :float_compare
    b, a = math_coerce other, :compare_error
    a <=> b
  end

  def ==(other)
    Ruby.primitive :float_equal
    begin
      b, a = math_coerce(other)
      return a == b
    rescue TypeError
      return other == self
    end
  end

  def eql?(other)
    Ruby.primitive :float_eql
    raise PrimitiveFailure, "Float#eql? primitive failed"
  end

  def nan?
    Ruby.primitive :float_isnan
    raise PrimitiveFailure, "Float#nan? primitive failed"
  end

  def infinite?
    Ruby.primitive :float_isinf
    raise PrimitiveFailure, "Float#infinite? primitive failed"
  end

  def finite?
    not (nan? or infinite?)
  end

  def to_f
    self
  end

  def to_i
    Ruby.primitive :float_to_i
    raise PrimitiveFailure, "Float#to_i primitive failed"
  end
  alias_method :to_int, :to_i
  alias_method :truncate, :to_i

  def to_s
    return (self < 0 ? "-Infinity" : "Infinity") if infinite?
    return "NaN" if nan?

    str = to_s_formatted "%#.15g"
    e = str.index('e') || str.size
    unless str[e-1].isdigit
      str = to_s_formatted "%#.14e"
      e = str.index('e') || str.size
    end
    str.gsub(/(\.\d+?)(0+)($|e[+-]\d*)/, '\1\3')
  end
  alias_method :inspect, :to_s

  def to_s_formatted(fmt)
    Ruby.primitive :float_to_s_formatted
    raise PrimitiveFailure, "String#to_s_formatted output exceeds buffer size"
  end
  private :to_s_formatted

  def round
    Ruby.primitive :float_round
  end
end


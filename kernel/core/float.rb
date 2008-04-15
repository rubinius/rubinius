# depends on: numeric.rb precision.rb class.rb

class Float < Numeric

  include Precision

  RADIX      = Platform::Float.RADIX
  ROUNDS     = Platform::Float.ROUNDS
  MIN        = Platform::Float.MIN
  MAX        = Platform::Float.MAX
  MIN_EXP    = Platform::Float.MIN_EXP
  MAX_EXP    = Platform::Float.MAX_EXP
  MIN_10_EXP = Platform::Float.MIN_10_EXP
  MAX_10_EXP = Platform::Float.MAX_10_EXP
  DIG        = Platform::Float.DIG
  MANT_DIG   = Platform::Float.MANT_DIG
  EPSILON    = Platform::Float.EPSILON
  STRLEN     = 32
  
  def self.induced_from(obj)
    case obj
    when Float, Bignum, Fixnum
      obj.to_f
    else
      raise TypeError, "failed to convert #{obj.class} into Float"
    end
  end
  
  def coerce(other)
    return [other, self] if other.__kind_of__ Float
    [Float(other), self]
  end
  
  def -@
    Ruby.primitive :float_uminus
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
    self.divmod(Float(other))[1]
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
  end
  
  def nan?
    Ruby.primitive :float_isnan
  end

  def infinite?
    Ruby.primitive :float_isinf
  end
  
  def finite?
    not (nan? or infinite?) 
  end

  def to_f
    self
  end
  
  def to_i
    Ruby.primitive :float_to_i
  end
  alias_method :to_int, :to_i
  alias_method :truncate, :to_i
  
  def to_s
    return (self < 0 ? "-Infinity" : "Infinity") if infinite?
    return "NaN" if nan?
    
    str = Platform::Float.to_s_formatted STRLEN, "%#.15g", self
    e = str.index('e') || str.size
    unless str[e-1].isdigit
      str = Platform::Float.to_s_formatted STRLEN, "%#.14e", self
      e = str.index('e') || str.size
    end
    str.gsub(/(\.\d+?)(0+)($|e[+-]\d*)/, '\1\3')
  end
  alias_method :inspect, :to_s

  def to_s_formatted(fmt)
    Platform::Float.to_s_formatted STRLEN, fmt, self
  end
  private :to_s_formatted
  
  def round
    Ruby.primitive :float_round
  end  
end

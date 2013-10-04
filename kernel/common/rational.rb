#
#   rational.rb -
#       $Release Version: 0.5 $
#       $Revision: 1.7 $
#       $Date: 1999/08/24 12:49:28 $
#       by Keiju ISHITSUKA(SHL Japan Inc.)
#

class Rational < Numeric
  attr_reader :numerator
  attr_reader :denominator

  def *(other)
    case other
    when Rational
      num = @numerator * other.numerator
      den = @denominator * other.denominator
      Rational(num, den)
    when Integer
      Rational(@numerator * other, @denominator)
    when Float
      Float(self) * other
    else
      a, b = other.coerce(self)
      a * b
    end
  end

  def **(other)
    if other.kind_of?(Rational) && other.denominator == 1
      other = other.numerator
    end

    case other
    when Fixnum
      if other > 0
        Rational(@numerator ** other, @denominator ** other)
      elsif other < 0
        raise ZeroDivisionError, "divided by 0" if self == 0
        Rational(@denominator ** -other, @numerator ** -other)
      elsif other == 0
        Rational.new(1, 1)
      end
    when Bignum
      if self == 0
        if other < 0
          raise ZeroDivisionError, "divided by 0"
        elsif other > 0
          return Rational(0)
        end
      elsif self == 1
        return Rational(1)
      elsif self == -1
        return Rational(other.even? ? 1 : -1)
      end

      Float(self) ** other
    when Float
      Float(self) ** other
    when Rational
      if self == 0 && other < 0
        raise ZeroDivisionError, "divided by 0"
      end

      Float(self) ** other
    else
      a, b = other.coerce(self)
      a ** b
    end
  end

  def +(other)
    case other
    when Rational
      num = @numerator * other.denominator + @denominator * other.numerator
      den = @denominator * other.denominator
      Rational(num, den)
    when Integer
      Rational(@numerator + other * @denominator, @denominator)
    when Float
      Float(self) + other
    else
      a, b = other.coerce(self)
      a + b
    end
  end

  def -(other)
    case other
    when Rational
      num = @numerator * other.denominator - @denominator * other.numerator
      den = @denominator * other.denominator
      Rational(num, den)
    when Integer
      Rational(@numerator - other * @denominator, @denominator)
    when Float
      Float(self) - other
    else
      a, b = other.coerce(self)
      a - b
    end
  end

  def <=>(other)
    case other
    when Rational
      diff = @numerator * other.denominator - @denominator * other.numerator
      diff <=> 0
    when Integer
      diff = @numerator - @denominator * other
      diff <=> 0
    when Float
      return Float(self) <=> other
    else
      if defined?(other.coerce)
        a, b = other.coerce(self)
        a <=> b
      else
        nil
      end
    end
  end

  def ==(other)
    case other
    when Rational
      @numerator == other.numerator && @denominator == other.denominator
    when Integer
      @numerator == other && @denominator == 1
    when Float
      Float(self) == other
    else
      other == self
    end
  end

  def abs
    (@numerator < 0) ? Rational(-@numerator, @denominator) : self
  end

  def ceil
    -(-@numerator / @denominator)
  end

  def coerce(other)
    case other
    when Integer
      return Rational.new(other, 1), self
    when Float
      return other, self.to_f
    else
      super
    end
  end

  def divide(other)
    case other
    when Rational
      num = @numerator * other.denominator
      den = @denominator * other.numerator
      Rational(num, den)
    when Integer
      raise ZeroDivisionError, "divided by 0" if other == 0
      Rational(@numerator, @denominator * other)
    when Float
      Float(self) / other
    else
      redo_coerced :/, other
    end
  end
  alias_method :/, :divide

  def floor
    @numerator / @denominator
  end

  def hash
    @numerator.hash ^ @denominator.hash
  end

  def inspect
    "(#{to_s})"
  end

  def rationalize(eps = undefined)
    return self if undefined.equal?(eps)

    e = eps.abs
    a = self - e
    b = self + e

    p0, p1, q0, q1 = 0, 1, 1, 0

    while true
      c = a.ceil

      break if c < b

      k = c - 1
      p2 = k * p1 + p0
      q2 = k * q1 + q0
      t = 1 / (b - k)
      b = 1 / (a - k)
      a = t

      p0, q0, p1, q1 = p1, q1, p2, q2
    end

    Rational(c * p1 + p0, c * q1 + q0)
  end

  def round(precision = 0)
    return 0 if @numerator == 0
    return @numerator if @denominator == 1

    adj = (10 ** precision.abs).to_i
    adj = Rational(1, adj) if precision < 0

    value = self * adj

    value = if self > 0
      (value + Rational(1,2)).floor
    else
      (value - Rational(1,2)).ceil
    end

    result = Rational(value, adj)

    return result.numerator if result.denominator == 1

    result
  end

  def to_f
    @numerator.to_f / @denominator.to_f
  end

  def to_i
    truncate
  end

  def to_r
    self
  end

  def to_s
    "#{@numerator.to_s}/#{@denominator.to_s}"
  end

  def truncate
    @numerator < 0 ? ceil : floor
  end

  def self.convert(num, den, mathn = true)
    if num.nil? || den.nil?
      raise TypeError, "cannot convert nil into Rational"
    end

    if num.kind_of?(Integer) && den.kind_of?(Integer)
      return reduce(num, den, mathn)
    end

    case num
    when Integer
      # nothing
    when Float, String, Complex
      num = num.to_r
    end

    case den
    when Integer
      # nothing
    when Float, String, Complex
      den = den.to_r
    end

    if den.equal?(1) && !num.kind_of?(Integer)
      return Rubinius::Type.coerce_to(num, Rational, :to_r)
    elsif num.kind_of?(Numeric) && den.kind_of?(Numeric) &&
        !(num.kind_of?(Integer) && den.kind_of?(Integer))
      return num / den
    end

    reduce(num, den)
  end
  private_class_method :convert

  def self.reduce(num, den, mathn = true)
    case num
    when Integer
      # nothing
    when Numeric
      num = num.to_i
    else
      raise TypeError, "numerator is not an Integer"
    end

    case den
    when Integer
      if den == 0
        raise ZeroDivisionError, "divided by 0"
      elsif den < 0
        num = -num
        den = -den
      end

      if den == 1
        return (mathn && Rubinius.mathn_loaded?) ? num : new(num, den)
      end
    when Numeric
      den = den.to_i
    else
      raise TypeError, "denominator is not an Integer"
    end

    gcd = num.gcd(den)
    num = num / gcd
    den = den / gcd

    return num if mathn && Rubinius.mathn_loaded? && den == 1

    new(num, den)
  end
  private_class_method :reduce

  def initialize(num, den)
    @numerator = num
    @denominator = den
  end
  private :initialize

  def marshal_dump
    ary = [@numerator, @denominator]

    instance_variables.each do |ivar|
      ary.instance_variable_set(ivar, instance_variable_get(ivar))
    end

    ary
  end
  private :marshal_dump

  def marshal_load(ary)
    @numerator, @denominator = ary

    ary.instance_variables.each do |ivar|
      instance_variable_set(ivar, ary.instance_variable_get(ivar))
    end

    self
  end
  private :marshal_load
end

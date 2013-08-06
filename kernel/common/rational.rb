# -*- encoding: us-ascii -*-

#
#   rational.rb -
#       $Release Version: 0.5 $
#       $Revision: 1.7 $
#       $Date: 1999/08/24 12:49:28 $
#       by Keiju ISHITSUKA(SHL Japan Inc.)
#

class Rational < Numeric

  def self.convert(num, den, mathn = true)
    if num.equal? nil or den.equal? nil
      raise TypeError, "cannot convert nil into Rational"
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

    if num.kind_of? Integer and den.kind_of? Integer
      return reduce(num, den, mathn)
    end

    if den.equal? 1 and !num.kind_of? Integer
      return Rubinius::Type.coerce_to num, Rational, :to_r
    else
      if num.kind_of? Numeric and den.kind_of? Numeric and
         !(num.kind_of? Integer and den.kind_of? Integer)
        return num / den
      end
    end

    reduce num, den
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
        return num if mathn && Rubinius.mathn_loaded?
        return new(num, den)
      end
    when Numeric
      den = den.to_i
    else
      raise TypeError, "denominator is not an Integer"
    end

    gcd = num.gcd den
    num = num.div gcd
    den = den.div gcd
    return num if mathn && Rubinius.mathn_loaded? && den == 1
    new(num, den)
  end

  private_class_method :reduce

  def initialize(num, den)
    @numerator = num
    @denominator = den
  end

  def + (a)
    if a.kind_of?(Rational)
      num = @numerator * a.denominator
      num_a = a.numerator * @denominator
      Rational(num + num_a, @denominator * a.denominator)
    elsif a.kind_of?(Integer)
      self + Rational.new(a, 1)
    elsif a.kind_of?(Float)
      Float(self) + a
    else
      x, y = a.coerce(self)
      x + y
    end
  end

  def - (a)
    if a.kind_of?(Rational)
      num = @numerator * a.denominator
      num_a = a.numerator * @denominator
      Rational(num - num_a, @denominator*a.denominator)
    elsif a.kind_of?(Integer)
      self - Rational.new(a, 1)
    elsif a.kind_of?(Float)
      Float(self) - a
    else
      x, y = a.coerce(self)
      x - y
    end
  end

  def * (a)
    if a.kind_of?(Rational)
      num = @numerator * a.numerator
      den = @denominator * a.denominator
      Rational(num, den)
    elsif a.kind_of?(Integer)
      self * Rational.new(a, 1)
    elsif a.kind_of?(Float)
      Float(self) * a
    else
      x, y = a.coerce(self)
      x * y
    end
  end

  def divide (a)
    if a.kind_of?(Rational)
      num = @numerator * a.denominator
      den = @denominator * a.numerator
      Rational(num, den)
    elsif a.kind_of?(Integer)
      raise ZeroDivisionError, "division by zero" if a == 0
      self / Rational.new(a, 1)
    elsif a.kind_of?(Float)
      Float(self) / a
    else
      redo_coerced :/, a
    end
  end

  alias_method :/, :divide

  def ** (other)
    if other.kind_of?(Rational) && other.denominator == 1
      other = other.numerator
    end

    if other.kind_of?(Fixnum)
      if self == 0 && other < 0
        raise ZeroDivisionError, "divided by 0"
      end
      if other > 0
        num = @numerator ** other
        den = @denominator ** other
      elsif other < 0
        num = @denominator ** -other
        den = @numerator ** -other
      elsif other == 0
        num = 1
        den = 1
      end
      Rational(num, den)
    elsif other.kind_of?(Bignum)
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
    elsif other.kind_of?(Float)
      Float(self) ** other
    elsif other.kind_of?(Rational)
      if self == 0 && other < 0
        raise ZeroDivisionError, "divided by 0"
      else
        Float(self) ** other
      end
    else
      x, y = other.coerce(self)
      x ** y
    end
  end

  def div(other)
    (self / other).floor
  end

  def % (other)
    if other == 0.0
      raise ZeroDivisionError, "division by zero"
    end
    value = (self / other).floor
    return self - other * value
  end

  def divmod(other)
    if other.is_a?(Float) && other == 0.0
      raise ZeroDivisionError, "division by zero"
    end
    value = (self / other).floor
    return value, self - other * value
  end

  def abs
    if @numerator > 0
      self
    else
      Rational(-@numerator, @denominator)
    end
  end

  def == (other)
    if other.kind_of?(Rational)
      @numerator == other.numerator and (@denominator == other.denominator or @numerator.zero?)
    elsif other.kind_of?(Integer)
      self == Rational.new(other, 1)
    elsif other.kind_of?(Float)
      Float(self) == other
    else
      other == self
    end
  end

  def <=> (other)
    if other.kind_of?(Rational)
      num = @numerator * other.denominator
      num_a = other.numerator * @denominator
      v = num - num_a
      if v > 0
        return 1
      elsif v < 0
        return  -1
      else
        return 0
      end
    elsif other.kind_of?(Integer)
      return self <=> Rational.new(other, 1)
    elsif other.kind_of?(Float)
      return Float(self) <=> other
    elsif defined? other.coerce
      x, y = other.coerce(self)
      return x <=> y
    else
      return nil
    end
  end

  def coerce(other)
    if other.kind_of?(Float)
      return other, self.to_f
    elsif other.kind_of?(Integer)
      return Rational.new(other, 1), self
    else
      super
    end
  end

  def floor
    @numerator.div(@denominator)
  end

  def ceil
    -((-@numerator).div(@denominator))
  end

  def truncate
    if @numerator < 0
      return -((-@numerator).div(@denominator))
    end
    @numerator.div(@denominator)
  end

  alias_method :to_i, :truncate

  def round
    if @numerator < 0
      num = -@numerator
      num = num * 2 + @denominator
      den = @denominator * 2
      -(num.div(den))
    else
      num = @numerator * 2 + @denominator
      den = @denominator * 2
      num.div(den)
    end
  end

  def to_f
    @numerator.to_f/@denominator.to_f
  end

  def to_s
    @numerator.to_s+"/"+@denominator.to_s
  end

  def to_r
    self
  end

  def inspect
    "(#{to_s})"
  end

  def hash
    @numerator.hash ^ @denominator.hash
  end

  attr_reader :numerator
  attr_reader :denominator

  private :initialize

  def rationalize(eps=undefined)
    if undefined.equal?(eps)
      self
    else
      e = eps.abs
      a = self - e
      b = self + e

      p0 = 0
      p1 = 1
      q0 = 1
      q1 = 0

      while true
        c = a.ceil
        break if c < b
        k = c - 1
        p2 = k * p1 + p0
        q2 = k * q1 + q0
        t = 1 / (b - k)
        b = 1 / (a - k)
        a = t
        p0 = p1
        q0 = q1
        p1 = p2
        q1 = q2
      end

      Rational(c * p1 + p0, c * q1 + q0)
    end
  end

  def marshal_dump
    ary = [numerator, denominator]
    instance_variables.each do |ivar|
      ary.instance_variable_set(ivar, instance_variable_get(ivar))
    end
    ary
  end

  def marshal_load(ary)
    @numerator, @denominator = ary
    ary.instance_variables.each do |ivar|
      instance_variable_set(ivar, ary.instance_variable_get(ivar))
    end
    self
  end

  #
  # Returns the truncated value, toward the nearest unit, based on the
  # precision.
  #
  # eg:
  #
  # Rational(1,2).round    #=> 1
  # Rational(-1,2).round   #=> -1
  # Rational(1,4).round(1) #=> Rational(3,10)
  #
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
end

class Numeric
  include Comparable

  def +@
    self
  end

  def -@
    0 - self
  end

  def +(other)
    b, a = math_coerce other
    a + b
  end

  def -(other)
    b, a = math_coerce other
    a - b
  end

  def *(other)
    b, a = math_coerce other
    a * b
  end

  def %(other)
    b, a = math_coerce other
    raise ZeroDivisionError, "divided by 0" unless b.__kind_of__(Float) or b != 0
    a % b
  end

  #--
  # see doc/compiler/plugins.txt regarding safe math compiler plugin
  #++

  def divide(other)
    b, a = math_coerce other
    raise ZeroDivisionError, "divided by 0" unless b.__kind_of__(Float) or b != 0
    a / b
  end
  alias_method :/, :divide

  def **(other)
    b, a = math_coerce other
    a ** b
  end

  def divmod(other)
    b, a = math_coerce other

    if other == 0
      raise FloatDomainError, "NaN" if other.__kind_of__ Float
      raise ZeroDivisionError, "divided by 0"
    end

    a.divmod b
  end

  def div(other)
    raise FloatDomainError, "NaN" if self == 0 && other.__kind_of__(Float) && other == 0
    b, a = math_coerce other
    (a / b).floor
  end

  def quo(other)
    if other.__kind_of__ Integer
      self / Float(other)
    else
      b, a = math_coerce other
      a / b
    end
  end

  def <(other)
    b, a = math_coerce other, :compare_error
    a < b
  end

  def <=(other)
    b, a = math_coerce other, :compare_error
    a <= b
  end

  def >(other)
    b, a = math_coerce other, :compare_error
    a > b
  end

  def >=(other)
    b, a = math_coerce other, :compare_error
    a >= b
  end

  def ==(other)
    return true if other.equal?(self)
    if other.kind_of? Numeric
      b, a = other.coerce self
      a == b
    else
      !!(other == self)
    end
  end

  def <=>(other)
    begin
      b, a = math_coerce other, :compare_error
      return a <=> b
    rescue ArgumentError
      return nil
    end
  end

  def step(limit, step=1, &block)
    return to_enum :step, limit unless block_given?

    raise ArgumentError, "step cannot be 0" if step == 0

    idx = self
    if idx.is_a?(Float) || limit.is_a?(Float) || step.is_a?(Float)
      idx, limit, step = FloatValue(idx), FloatValue(limit), FloatValue(step)
    end

    cmp = step > 0 ? :> : :<
    until idx.send(cmp,limit)
      yield idx
      idx += step
    end

    return self
  end

  def truncate
    Float(self).truncate
  end

  # Delegate #to_int to #to_i in subclasses
  def to_int
    self.to_i
  end

  # Delegate #modulp to #% in subclasses
  def modulo(other)
    self % other
  end

  def integer?
    false
  end

  def zero?
    self == 0
  end

  def nonzero?
    zero? ? nil : self
  end

  def round
    self.to_f.round
  end

  def abs
    self < 0 ? -self : self
  end

  def floor
    int = self.to_i
    if self == int or self > 0
      int
    else
      int - 1
    end
  end

  def ceil
    int = self.to_i
    if self == int or self < 0
      int
    else
      int + 1
    end
  end

  def remainder(other)
    b, a = math_coerce other
    mod = a % b

    if mod != 0 && (a < 0 && b > 0 || a > 0 && b < 0)
      mod - b
    else
      mod
    end
  end

  #--
  # We deviate from MRI behavior here because we ensure that Fixnum op Bignum
  # => Bignum (possibly normalized to Fixnum)
  #
  # Note these differences on MRI, where a is a Fixnum, b is a Bignum
  #
  #   a.coerce b => [Float, Float]
  #   b.coerce a => [Bignum, Bignum]
  #++

  def coerce(other)
    [Float(other), Float(self)]
  end

  ##
  # This method mimics the semantics of MRI's do_coerce function
  # in numeric.c. Note these differences between it and #coerce:
  #
  #   1.2.coerce("2") => [2.0, 1.2]
  #   1.2 + "2" => TypeError: String can't be coerced into Float
  #
  # See also Integer#coerce

  def math_coerce(other, error=:coerce_error)
    begin
      values = other.coerce(self)
    rescue
      send error, other
    end

    unless values.__kind_of__(Array) && values.length == 2
      raise TypeError, "coerce must return [x, y]"
    end

    return values[1], values[0]
  end
  private :math_coerce

  def coerce_error(other)
    raise TypeError, "#{other.class} can't be coerced into #{self.class}"
  end
  private :coerce_error

  def compare_error(other)
    raise ArgumentError, "comparison of #{self.class} with #{other.class} failed"
  end
  private :compare_error
end


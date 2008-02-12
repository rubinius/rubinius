# depends on: class.rb comparable.rb

class Numeric
  include Comparable

  def +(other)
    b, a = math_coerce(other)
    a + b
  end
  
  def -(other)
    b, a = math_coerce(other)
    a - b
  end
  
  def *(other)
    b, a = math_coerce(other)
    a * b
  end
  
  # see README-DEVELOPERS regarding safe math compiler plugin
  def divide(other)
    b, a = math_coerce(other)
    raise ZeroDivisionError, "divided by 0" unless b.kind_of?(Float) or b != 0
    a / b
  end
  alias_method :/, :divide
  
  def **(other)
    b, a = math_coerce(other)
    a ** b
  end
  
  def %(other)
    b, a = math_coerce(other)
    raise ZeroDivisionError, "divided by 0" unless b.kind_of?(Float) or b != 0
    a % b
  end
   
  def &(other)
    self & Type.coerce_to(other, Integer, :to_int)
  end
   
  def |(other)
    self | Type.coerce_to(other, Integer, :to_int)
  end
  
  def ^(other)
    self ^ Type.coerce_to(other, Integer, :to_int)
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

  def +@
    self
  end

  def -@
    0 - self
  end

  def integer?
    false
  end
 
  def div(other)
    raise FloatDomainError, "NaN" if self == 0 && other.is_a?(Float) && other == 0
    b, a = math_coerce(other)
    (a / b).floor
  end

  def quo(other)
    if other.is_a?(Integer)
      self / Float(other)
    else
      b, a = math_coerce(other)
      a / b
    end
  end

  def divmod(other)
    b, a = math_coerce(other)
    
    if other == 0
      raise FloatDomainError, "NaN" if other.is_a?(Float)
      raise ZeroDivisionError, "divided by 0"
    end
    
    a.divmod(b)
  end
  
  def round
    self.to_f.round
  end
  
  def ==(other)
    begin
      b, a = math_coerce(other)
      return a == b
    rescue TypeError
      return other == self
    end
  end
  
  def <=>(other)
    begin
      b, a = math_coerce(other)
      return a <=> b
    rescue TypeError
      return nil
    end
  end
  
  def zero?
    self == 0
  end

  def nonzero?
    if zero?
      nil
    else
      self
    end
  end

  def remainder(other)
    b, a = math_coerce(other)
    mod = a % b

    if mod != 0 && (a < 0 && b > 0 || a > 0 && b < 0)
      mod - b
    else
      mod
    end
  end

  def step(limit, step=1, &block)
    raise ArgumentError, "step cannot be 0" if step == 0
    limit,step = step.coerce(limit)
    # FIXME: why is this not covered by the block parameter above?
    raise LocalJumpError, "no block given" unless block_given?
    idx,step = step.coerce(self)
    cmp = step > 0 ? :<= : :>=
    while (idx.send(cmp,limit))
      yield(idx)
      idx += step
    end
    return self
  rescue TypeError => e
    raise ArgumentError, e.message
  end

  # This method mimics the semantics of MRI's do_coerce function
  # in numeric.c. Note these differences between it and #coerce:
  #   1.2.coerce("2") => [2.0, 1.2]
  #   1.2 + "2" => TypeError: String can't be coerced into Float
  #
  # We do not attempt to produce the exact same exception message
  # as MRI, so please do not edit it to match.
  #
  # See also our Numeric#coerce
  def math_coerce(other, error=:coerce_error)
    begin
      values = other.coerce(self)
    rescue
      send error, other
    end
    
    unless values.is_a?(Array) && values.length == 2
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

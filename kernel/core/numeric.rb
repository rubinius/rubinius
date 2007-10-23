# depends on: comparable.rb

class Numeric
  include Comparable

  def +(other)
    b, a = self.do_coerce(other, true)
    a + b
  end
  
  def -(other)
    b, a = self.do_coerce(other, true)
    a - b
  end
  
  def *(other)
    b, a = self.do_coerce(other, true)
    a * b
  end
  
  def /(other)
    b, a = self.do_coerce(other, true)
    raise ZeroDivisionError, "divided by 0" unless Float === b or b != 0
    a / b
  end
  
  def **(other)
    b, a = self.do_coerce(other, true)
    a ** b
  end
  
  def %(other)
    b, a = self.do_coerce(other, true)
    raise ZeroDivisionError, "divided by 0" unless Float === b or b != 0
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
    (self / other).floor
  end

  def quo(other)
    if other.is_a?(Fixnum)
      self / Float(other)
    else
      b, a = self.do_coerce(other, true)
      a / b
    end
  end

  def divmod(other)
    b, a = self.do_coerce(other, true)
    
    if other == 0
      raise FloatDomainError, "NaN" if other.is_a?(Float)
      raise ZeroDivisionError, "divided by 0"
    end
    
    a.divmod(b)
  end
  
  def ==(other)
    if values = self.do_coerce(other, false)
      return values[1] == values[0]
    else
      return other == self
    end
  end
  
  def <=>(other)
    if values = self.do_coerce(other)
      values[1] <=> values[0]
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

  # Little helper to ease coercing
  def do_coerce(other, raise_error = false)
    begin
      values = other.coerce(self)
    rescue
      if raise_error
        message = "#{other.is_a?(Symbol) ? other.inspect : other.class} can't be coerced into #{self.class}"
        raise TypeError, message 
      end
      return false
    end
    
    unless values.is_a?(Array) && values.length == 2
      raise TypeError, "coerce must return [x, y]" if raise_error
      return false
    end
    
    return values[1], values[0]
  end
end


class Numeric
  include Comparable

  def ==(other)
    b, a = self.coerce(other)
    a == b
  end
  
  def +(other)
    b, a = self.coerce(other)
    a + b
  end
  
  def -(other)
    b, a = self.coerce(other)
    a - b
  end
  
  def *(other)
    b, a = self.coerce(other)
    a * b
  end
  
  def /(other)
    b, a = self.coerce(other)
    raise ZeroDivisionError, "Divide by 0" unless Float === b or b != 0
    a / b
  end
  
  def **(other)
    b, a = self.coerce(other)
    a ** b
  end
  
  def %(other)
    b, a = self.coerce(other)
    raise ZeroDivisionError, "Divide by 0" unless Float === b or b != 0
    a % b
  end
  
  def &(other)
    self & Integer(other)
  end
  
  def |(other)
    self | Integer(other)
  end
  
  def ^(other)
    self ^ Integer(other)
  end

  def abs
    return -self if self < 0
    self
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
  
  def divmod(other)
    b, a = self.coerce(other)
    raise ZeroDivisionError, "Divide by 0" unless Float === b or b != 0
    a.divmod(b)
  end
  
  def ==(other)
    begin
      b, a = self.coerce(other)
      return a == b
    rescue TypeError
      return false
    end
  end
  
  def <=>(other)
    b, a = self.coerce(other)
    a <=> b
  end

  def coerce(other)
    Ruby.primitive :numeric_coerce
    self.class == other.class ? [other, self] : [Float(other), Float(self)]
  end

  def zero?
    self == 0
  end

  def step(limit, step=1)
    raise "Float.step not implemented" if Float === self || Float === limit || Float === step
    return self if ((self > limit && step > 0) || (self < limit && step < 0))
    raise LocalJumpError, "no block given" if !block_given?
    raise ArgumentError, "step cannot be 0" if step == 0
    idx = self
    while (step > 0 ? idx <= limit : idx >= limit) == true
      yield(idx)
      idx += step
    end
  end
end


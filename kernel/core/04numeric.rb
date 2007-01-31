class Numeric
  include Comparable
  
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
    a / b
  end
  
  def **(other)
    b, a = self.coerce(other)
    a ** b
  end
  
  def %(other)
    b, a = self.coerce(other)
    a % b
  end

  def abs
    return -self if self < 0
    self
  end

  def floor
    if self >= 0
      self.to_i
    else
      self.to_i - 1
    end
  end

  def ceil
    int = self.to_i
    if self < 0 or self == int
      int
    else
      self.to_i + 1
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
    a.divmod(b)
    #[(a / b).floor, a % b]
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
end


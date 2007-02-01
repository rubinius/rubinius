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
end


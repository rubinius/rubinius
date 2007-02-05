class Integer < Numeric
  def self.induced_from(o)
    return o.to_i if o.respond_to?(:to_i)
    raise TypeError, "can't convert #{o.class} into Integer"
  end

  def times
    i = 0
    while i < self
      yield i
      i += 1
    end
    self
  end
  
  def upto(val)
    i = self
    while i <= val
      yield i
      i += 1
    end
    return self
  end
  
  def downto(val)
    i = self
    while i >= val
      yield i
      i -= 1
    end
    return self
  end

  def to_i
    self
  end

  def chr
    raise RangeError.new("#{self} is out of the valid character range") if self > 255 || self < 0
    a = "x"                     # This automatically does a dup
    a.data.set_byte(0, self)
    a
  end
  
  def isspace
    self == ?\s or self == ?\n or self == ?\t or self == ?\r or self == ?\f or self == ?\v
  end

  def [](i)
    if i < 0
      0
    else
      (self >> i) & 1
    end
  end

  def **(exp)
    if exp < 0 || !(Integer === exp)
      Float(self)**Float(exp)
    else
      out = 1
      base = self
      while exp > 0
        if (exp & 1) != 0
          out *= base
          exp -= 1
        else
          base *= base
          exp >>= 1
        end
      end
      out
    end
  end

  def quo(other)
    Float(self) / Float(other)
  end
  
end

class Numeric
  include Comparable

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

  def <=>(other)
    Ruby.primitive :numeric_compare
  end
  
  def coerce(other)
    Ruby.primitive :numeric_coerce
  end
end


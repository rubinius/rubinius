class Integer < Numeric
  include CType
  
  def &(other)
    b, a = self.coerce(other)
    a & b
  end

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
  alias to_int to_i
  alias round to_i
  alias truncate to_i

  def chr
    raise RangeError.new("#{self} is out of the valid character range") if self > 255 || self < 0
    a = "x"                     # This automatically does a dup
    a.data.set_byte(0, self)
    a
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
  
  def next
    self + 1
  end
  alias succ next

  def integer?
    true
  end

  # returns minimum bit storage required for integer in (signed int) binary format
  # NOTE: rshift would probably be slightly more efficient but since i'm
  # probably going to use this to simplify the complex behavior of
  # ruby's << and >> it would defeat the purpose by creating a circular
  # dependency.
  # TODO: convert algorithm to primitive so no circular dependency?
  def bits(int = self)
    if int.zero?
      1 # sign bit storage
    elsif int > 0
      bits(int / 2) + 1 # could use >> in primitive
    else
      bits(~int / 2) + 1 # could use >> in primitive
    end
  end
end


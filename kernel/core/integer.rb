# depends on: numeric.rb ctype.rb precision.rb

class Integer < Numeric
  include CType
  include Precision

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
  alias_method :to_int, :to_i
  alias_method :round, :to_i
  alias_method :truncate, :to_i

  def chr
    raise RangeError.new("#{self} is out of the valid character range") if self > 255 || self < 0
    a = "x"
    a[0] = self
    a
  end
  
  def [](index)
    return 0 if index.is_a?(Bignum)
    index = Type.coerce_to(index, Integer, :to_int)
    index < 0 ? 0 : (self >> index) & 1
  end

  def **(exp)
    if !exp.is_a?(Integer)
      b, a = self.do_coerce(exp, true)
      return a ** b
    end
    
    return 1 if exp == 0
    return self if exp == 1 || self == 0 || self == 1
    return b % 2 == 0 ? 1 : -1 if self == -1
    
    if exp < 0
      Float(self) ** Float(exp)
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

  def next
    self + 1
  end
  alias_method :succ, :next

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

  # assumes self is positive
  def interpret_as_float()
    sign = (2**31 & self != 0) ? -1 : 1
    expo = ((0xFF * 2**23) & self) >> 23
    frac = (2**23 - 1) & self
    if expo == 0 and frac == 0
      sign.to_f * 0.0  # zero
    elsif expo == 0
      sign * 2**(expo - 126) * (frac.to_f / 2**23.to_f)  # denormalized
    elsif expo == 0xFF and frac == 0
      sign.to_f / 0.0  # Infinity
    elsif expo == 0xFF
      0.0 / 0.0  # NaN
    else
      sign * 2**(expo - 127) * (1.0 + (frac.to_f / 2**23.to_f))  # normalized
    end
  end

  # assumes self is positive
  def interpret_as_double()
    sign = (2**63 & self != 0) ? -1 : 1
    expo = ((0x7FF * 2**52) & self) >> 52
    frac = (2**52 - 1) & self
    if expo == 0 and frac == 0
      sign.to_f * 0.0  # zero
    elsif expo == 0
      sign * 2**(expo - 1022) * (frac.to_f / 2**52.to_f)  # denormalized
    elsif expo == 0x7FF and frac == 0
      sign.to_f / 0.0  # Infinity
    elsif expo == 0x7FF
      0.0 / 0.0  # NaN
    else
      sign * 2**(expo - 1023) * (1.0 + (frac.to_f / 2**52.to_f))  # normalized
    end
  end
end


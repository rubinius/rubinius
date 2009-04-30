class Integer < Numeric
  include CType
  include Precision

  def self.induced_from(obj)
    case obj
    when Fixnum, Bignum
      obj
    when Float
      obj.to_i
    else
      raise TypeError, "failed to convert #{obj.class} into Integer"
    end
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

  def <<(other)
    other = Type.coerce_to(other, Integer, :to_int)
    unless other.is_a?(Fixnum)
      raise RangeError, "Object is out of range for a Fixnum"
    end
    self << other
  end

  def >>(other)
    other = Type.coerce_to(other, Integer, :to_int)
    if other > self.abs || other.is_a?(Bignum)
      return  0 if self >= 0
      return -1 if self <  0
    end
    self >> other
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
    String.pattern 1, self
  end
  
  def [](index)
    return 0 if index.is_a?(Bignum)
    index = Type.coerce_to(index, Integer, :to_int)
    index < 0 ? 0 : (self >> index) & 1
  end

  # FIXME: implement a fast way to calculate bignum exponents
  def **(exp)
    if exp.is_a?(Bignum)
      raise TypeError, "Bignum exponent #{exp} too large"
    end
    super(exp)
  end

  def next
    self + 1
  end
  alias_method :succ, :next

  def integer?
    true
  end

  ##
  # Returns the minimum number of bits required for integer in (signed int)
  # binary format
  #--
  # NOTE: rshift would probably be slightly more efficient but since i'm
  # probably going to use this to simplify the complex behavior of
  # ruby's << and >> it would defeat the purpose by creating a circular
  # dependency.
  #
  # TODO: convert algorithm to primitive so no circular dependency?
  #++

  def bits(int = self)
    num_bits = 1 # sign bit storage

    if int < 0
      int = ~int

      num_bits += 1
      int /= 2 # could use >> in primitive
    end

    while int != 0
      num_bits += 1
      int /= 2 # could use >> in primitive
    end

    num_bits
  end
end


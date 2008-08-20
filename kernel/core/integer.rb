# depends on: class.rb numeric.rb ctype.rb precision.rb

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
    String.template 1, self
  end
  
  def [](index)
    return 0 if index.is_a?(Bignum)
    index = Type.coerce_to(index, Integer, :to_int)
    index < 0 ? 0 : (self >> index) & 1
  end

  def **(exp)
    if !exp.is_a?(Integer)
      b, a = math_coerce(exp)
      return a ** b
    end
    
    return 1 if exp == 0
    return self if exp == 1 || self == 0 || self == 1
    return exp % 2 == 0 ? 1 : -1 if self == -1
    
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
    if int.zero?
      1 # sign bit storage
    elsif int > 0
      bits(int / 2) + 1 # could use >> in primitive
    else
      bits(~int / 2) + 1 # could use >> in primitive
    end
  end
end


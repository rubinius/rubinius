class Integer < Numeric
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
    self & Rubinius::Type.coerce_to(other, Integer, :to_int)
  end
   
  def |(other)
    self | Rubinius::Type.coerce_to(other, Integer, :to_int)
  end
  
  def ^(other)
    self ^ Rubinius::Type.coerce_to(other, Integer, :to_int)
  end

  def to_i
    self
  end

  alias_method :to_int, :to_i
  alias_method :round, :to_i
  alias_method :truncate, :to_i
  alias_method :ceil, :to_i
  alias_method :floor, :to_i

  def chr
    raise RangeError.new("#{self} is out of the valid character range") if self > 255 || self < 0
    String.pattern 1, self
  end
  
  def [](index)
    index = Rubinius::Type.coerce_to(index, Integer, :to_int)
    return 0 if index.is_a?(Bignum)
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

  # Returns true if int is an even number.
  def even?
    self & 1 == 0
  end

  # Returns true if int is an odd number.
  def odd?
    self & 1 == 1
  end

  # Returns the int itself.
  def ord
    self
  end

  # Returns the Integer equal to int - 1.
  def pred
    self - 1
  end

  def times
    return to_enum(:times) unless block_given?

    i = 0
    while i < self
      yield i
      i += 1
    end
    self
  end

  def upto(val)
    return to_enum(:upto, val) unless block_given?

    i = self
    while i <= val
      yield i
      i += 1
    end
    self
  end

  def downto(val)
    return to_enum(:downto, val) unless block_given?

    i = self
    while i >= val
      yield i
      i -= 1
    end
    self
  end
end


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
    if self > 255 || self < 0
      raise RangeError.new("#{self} is out of the valid character range")
    end
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

  #
  # In an integer, the value _is_ the numerator of its rational equivalent.
  # Therefore, this method returns +self+.
  #
  def numerator
    self
  end

  #
  # In an integer, the denominator is 1.  Therefore, this method returns 1.
  #
  def denominator
    1
  end

  #
  # Returns a Rational representation of this integer.
  #
  def to_r
    Rational(self, 1)
  end

  #
  # Returns the <em>greatest common denominator</em> of the two numbers (+self+
  # and +n+).
  #
  # Examples:
  #   72.gcd 168           # -> 24
  #   19.gcd 36            # -> 1
  #
  # The result is positive, no matter the sign of the arguments.
  #
  def gcd(other)
    raise TypeError, "Expected Integer but got #{other.class}" unless other.kind_of?(Integer)
    min = self.abs
    max = other.abs
    while min > 0
      tmp = min
      min = max % min
      max = tmp
    end
    max
  end

  #
  # Returns the <em>lowest common multiple</em> (LCM) of the two arguments
  # (+self+ and +other+).
  #
  # Examples:
  #   6.lcm 7        # -> 42
  #   6.lcm 9        # -> 18
  #
  def lcm(other)
    raise TypeError, "Expected Integer but got #{other.class}" unless other.kind_of?(Integer)
    if self.zero? or other.zero?
      0
    else
      (self.div(self.gcd(other)) * other).abs
    end
  end

  #
  # Returns the GCD _and_ the LCM (see #gcd and #lcm) of the two arguments
  # (+self+ and +other+).  This is more efficient than calculating them
  # separately.
  #
  # Example:
  #   6.gcdlcm 9     # -> [3, 18]
  #
  def gcdlcm(other)
    gcd = self.gcd(other)
    if self.zero? or other.zero?
      [gcd, 0]
    else
      [gcd, (self.div(gcd) * other).abs]
    end
  end
end


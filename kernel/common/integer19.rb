class Integer
  alias_method :magnitude, :abs

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

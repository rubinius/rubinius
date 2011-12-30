class Integer
  def chr(enc=undefined)
    if self < 0 || (self & 0xffff_ffff) != self
      raise RangeError, "#{self} is outside of the valid character range"
    end

    if enc.equal? undefined
      if 0xff < self
        enc = Encoding.default_internal
        if enc.nil?
          raise RangeError, "#{self} is outside of the valid character range"
        end
      elsif self < 0x80
        return String.pattern(1, self)
      else
        enc = Encoding::ASCII_8BIT
      end
    else
      enc = Rubinius::Type.coerce_to_encoding enc
    end

    String.from_codepoint self, enc
  end

  def round(ndigits=undefined)
    return self if ndigits.equal? undefined

    if Rubinius::Type.object_kind_of?(ndigits, Numeric)
      begin
        Rubinius::Type.coerce_to(ndigits, Fixnum, :to_int)
      rescue TypeError => e
        raise RangeError, e.message
      end
    end

    ndigits = Rubinius::Type.coerce_to(ndigits, Integer, :to_int)

    if ndigits > 0
      to_f
    elsif ndigits == 0
      self
    else
      ndigits = -ndigits

      # We want to return 0 if 10 ** ndigits / 2 > self.abs, or, taking
      # log_256 of both sides, if log_256(10 ** ndigits / 2) > self.size.
      # We have log_256(10) > 0.415241 and log_256(2) = 0.125, so:
      return 0 if 0.415241 * ndigits - 0.125 > size

      f = 10 ** ndigits
      h = f / 2
      r = self % f
      n = self - r

      unless self < 0 ? r <= h : r < h
        n = n + f
      end

      n
    end
  end

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

  def rationalize(eps = nil)
    Rational(self, 1)
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

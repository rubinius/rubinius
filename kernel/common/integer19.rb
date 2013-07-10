# -*- encoding: us-ascii -*-

class Integer
  def chr(enc=undefined)
    if self < 0 || (self & 0xffff_ffff) != self
      raise RangeError, "#{self} is outside of the valid character range"
    end

    if undefined.equal? enc
      if 0xff < self
        enc = Encoding.default_internal
        if enc.nil?
          raise RangeError, "#{self} is outside of the valid character range"
        end
      elsif self < 0x80
        enc = Encoding::US_ASCII
      else
        enc = Encoding::ASCII_8BIT
      end
    else
      enc = Rubinius::Type.coerce_to_encoding enc
    end

    String.from_codepoint self, enc
  end

  def round(ndigits=undefined)
    return self if undefined.equal? ndigits

    if ndigits.kind_of? Numeric
      if ndigits > Fixnum::MAX or ndigits < Fixnum::MIN
        raise RangeError, "precision is outside of the range of Fixnum"
      end
    end

    ndigits = Rubinius::Type.coerce_to ndigits, Integer, :to_int

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

      if kind_of? Fixnum and f.kind_of? Fixnum
        x = self < 0 ? -self : self
        x = (x + f / 2) / f * f
        x = -x if self < 0
        return x
      end

      return 0 if f.kind_of? Float

      h = f / 2
      r = self % f
      n = self - r

      unless self < 0 ? r <= h : r < h
        n += f
      end

      n
    end
  end

  alias_method :magnitude, :abs

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

  def numerator
    self
  end

  def denominator
    1
  end

  def to_r
    Rational(self, 1)
  end

  def lcm(other)
    raise TypeError, "Expected Integer but got #{other.class}" unless other.kind_of?(Integer)
    if self.zero? or other.zero?
      0
    else
      (self.div(self.gcd(other)) * other).abs
    end
  end

  def gcdlcm(other)
    gcd = self.gcd(other)
    if self.zero? or other.zero?
      [gcd, 0]
    else
      [gcd, (self.div(gcd) * other).abs]
    end
  end
end

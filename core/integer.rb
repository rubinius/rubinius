class Integer < Numeric
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

  # bitwise binary operators

  def &(other)
    Rubinius.asm do
      op = new_label
      done = new_label

      r0 = new_register
      r1 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, op
      goto done

      op.set!
      r_load_int r0, r0
      r_load_int r1, r1

      n_iand r0, r0, r1

      r_store_int r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    other = Rubinius::Type.coerce_to_bitwise_operand other

    other & self
  end

  def |(other)
    Rubinius.asm do
      op = new_label
      done = new_label

      r0 = new_register
      r1 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, op
      goto done

      op.set!
      r_load_int r0, r0
      r_load_int r1, r1

      n_ior r0, r0, r1

      r_store_int r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    other = Rubinius::Type.coerce_to_bitwise_operand other

    other | self
  end

  def ^(other)
    Rubinius.asm do
      op = new_label
      done = new_label

      r0 = new_register
      r1 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, op
      goto done

      op.set!
      r_load_int r0, r0
      r_load_int r1, r1

      n_ixor r0, r0, r1

      r_store_int r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    other = Rubinius::Type.coerce_to_bitwise_operand other

    other ^ self
  end

  def <<(other)
    Rubinius.asm do
      shr = new_label
      neg = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, neg
      goto done

      neg.set!
      r_load_int r2, r1
      r_load_0 r3

      n_ige r3, r2, r3
      b_if r3, shr

      n_ineg r1, r2
      r_store_int r1, r1
      n_ishr_o r0, r0, r1
      r_ret r0

      shr.set!
      n_ishl_o r0, r0, r1
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    other = Rubinius::Type.coerce_to other, Integer, :to_int

    self >> -other if other < 0

    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self << other
  end

  def >>(other)
    Rubinius.asm do
      shr = new_label
      neg = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, neg
      goto done

      neg.set!
      r_load_int r2, r1
      r_load_0 r3

      n_ige r3, r2, r3
      b_if r3, shr

      n_ineg r1, r2
      r_store_int r1, r1
      n_ishl_o r0, r0, r1
      r_ret r0

      shr.set!
      n_ishr_o r0, r0, r1
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    other = Rubinius::Type.coerce_to other, Integer, :to_int

    self << -other if other < 0

    unless other.kind_of? Fixnum
      raise RangeError, "argument is out of range for a Fixnum"
    end

    self >> other
  end

  def to_i
    self
  end

  alias_method :to_int, :to_i
  alias_method :truncate, :to_i
  alias_method :ceil, :to_i
  alias_method :floor, :to_i

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
      if ndigits > Fixnum::MAX or ndigits <= Fixnum::MIN
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

  def even?
    self & 1 == 0
  end

  def odd?
    self & 1 == 1
  end

  def ord
    self
  end

  def pred
    self - 1
  end

  def times
    return to_enum(:times) { self } unless block_given?

    i = 0
    while i < self
      yield i
      i += 1
    end
    self
  end

  def upto(val)
    return to_enum(:upto, val) { self <= val ? val - self + 1 : 0 } unless block_given?

    i = self
    while i <= val
      yield i
      i += 1
    end
    self
  end

  def downto(val)
    return to_enum(:downto, val) { self >= val ? self - val + 1 : 0 } unless block_given?

    i = self
    while i >= val
      yield i
      i -= 1
    end
    self
  end
end

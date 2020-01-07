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

  # binary math operators

  def +(other)
    Rubinius.asm do
      int = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_eadd r0, r0, r1
      goto val

      flt.set!
      n_dadd r0, r0, r1
      r_store_float r0, r0
      goto val

      int.set!
      n_iadd_o r0, r0, r1

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    redo_coerced :+, other
  end

  def -(other)
    Rubinius.asm do
      int = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_esub r0, r0, r1
      goto val

      flt.set!
      n_dsub r0, r0, r1
      r_store_float r0, r0
      goto val

      int.set!
      n_isub_o r0, r0, r1

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    redo_coerced :-, other
  end

  def *(other)
    Rubinius.asm do
      int = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_emul r0, r0, r1
      goto val

      flt.set!
      n_dmul r0, r0, r1
      r_store_float r0, r0
      goto val

      int.set!
      n_imul_o r0, r0, r1

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    redo_coerced :*, other
  end

  def /(other)
    Rubinius.asm do
      int = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_ediv r0, r0, r1
      goto val

      flt.set!
      n_ddiv r0, r0, r1
      r_store_float r0, r0
      goto val

      int.set!
      n_idiv_o r0, r0, r1

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    redo_coerced :/, other
  end

  alias_method :divide, :/

  def %(other)
    Rubinius.asm do
      int = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_emod r0, r0, r1
      goto val

      flt.set!
      n_dmod r0, r0, r1
      r_store_float r0, r0
      goto val

      int.set!
      n_imod_o r0, r0, r1

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    redo_coerced :%, other
  end

  alias_method :modulo, :%

  # comparison operators

  def !=(other)
    Rubinius.asm do
      cmp = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, cmp

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_ene r0, r0, r1
      goto val

      flt.set!
      n_dne r0, r0, r1
      goto val

      cmp.set!
      n_ine r0, r0, r1

      val.set!
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    other != self ? true : false
  end

  def ==(other)
    Rubinius.asm do
      cmp = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, cmp

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_eeq r0, r0, r1
      goto val

      flt.set!
      n_deq r0, r0, r1
      goto val

      cmp.set!
      n_ieq r0, r0, r1

      val.set!
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    other == self ? true : false
  end

  alias_method :===, :==

  def <(other)
    Rubinius.asm do
      cmp = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, cmp

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_elt r0, r0, r1
      goto val

      flt.set!
      n_dlt r0, r0, r1
      goto val

      cmp.set!
      n_ilt r0, r0, r1

      val.set!
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a < b ? true : false
  end

  def <=(other)
    Rubinius.asm do
      cmp = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, cmp

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_ele r0, r0, r1
      goto val

      flt.set!
      n_dle r0, r0, r1
      goto val

      cmp.set!
      n_ile r0, r0, r1

      val.set!
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a <= b ? true : false
  end

  def >(other)
    Rubinius.asm do
      cmp = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, cmp

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_egt r0, r0, r1
      goto val

      flt.set!
      n_dgt r0, r0, r1
      goto val

      cmp.set!
      n_igt r0, r0, r1

      val.set!
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a > b ? true : false
  end

  def >=(other)
    Rubinius.asm do
      cmp = new_label
      flt = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, cmp

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      n_ege r0, r0, r1
      goto val

      flt.set!
      n_dge r0, r0, r1
      goto val

      cmp.set!
      n_ige r0, r0, r1

      val.set!
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a >= b ? true : false
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

class Float < Numeric
  def **(other)
    Rubinius.primitive :float_pow

    if other.is_a?(Float) && self < 0 && other != other.round
      return Complex.new(self, 0) ** other
    end

    b, a = math_coerce other
    a ** b
  end

  def imaginary
    0
  end

  def numerator
    if nan?
      NAN
    elsif infinite? == 1
      INFINITY
    elsif infinite? == -1
      -INFINITY
    else
      super
    end
  end

  def denominator
    if infinite? || nan?
      1
    else
      super
    end
  end

  def to_r
    f, e = Math.frexp self
    f = Math.ldexp(f, MANT_DIG).to_i
    e -= MANT_DIG

    (f * (RADIX ** e)).to_r
  end

  def arg
    if nan?
      self
    elsif signbit?
      Math::PI
    else
      0
    end
  end
  alias_method :angle, :arg
  alias_method :phase, :arg

  def rationalize(eps=undefined)
    if undefined.equal?(eps)
      f, n = Math.frexp self
      f = Math.ldexp(f, Float::MANT_DIG).to_i
      n -= Float::MANT_DIG

      Rational.new(2 * f, 1 << (1 - n)).rationalize(Rational.new(1, 1 << (1 - n)))
    else
      to_r.rationalize(eps)
    end
  end

  def round(ndigits=0)
    ndigits = Rubinius::Type.coerce_to(ndigits, Integer, :to_int)

    if ndigits == 0
      return Rubinius.invoke_primitive :float_round, self
    elsif ndigits < 0
      return truncate.round ndigits
    end

    return self if infinite? or nan?

    _, exp = Math.frexp(self)

    if ndigits >= (Float::DIG + 2) - (exp > 0 ? exp / 4 : exp / 3 - 1)
      return self
    end

    if ndigits < -(exp > 0 ? exp / 3 + 1 : exp / 4)
      return 0.0
    end

    f = 10**ndigits
    Rubinius.invoke_primitive(:float_round, self * f) / f.to_f
  end
  def coerce(other)
    return [other, self] if other.kind_of? Float
    [Float(other), self]
  end

  def -@
    Rubinius.primitive :float_neg
    raise PrimitiveFailure, "Float#-@ primitive failed"
  end

  def abs
    FFI::Platform::Math.fabs(self)
  end

  alias_method :magnitude, :abs

  def signbit?
    Rubinius.primitive :float_signbit_p
    raise PrimitiveFailure, "Float#signbit? primitive failed"
  end

  def +(other)
    Rubinius.asm do
      flt = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt
      goto done

      flt.set!
      n_dadd r0, r0, r1
      r_store_float r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other
    a + b
  end

  def -(other)
    Rubinius.asm do
      flt = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt
      goto done

      flt.set!
      n_dsub r0, r0, r1
      r_store_float r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other
    a - b
  end

  def *(other)
    Rubinius.asm do
      flt = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt
      goto done

      flt.set!
      n_dmul r0, r0, r1
      r_store_float r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other
    a * b
  end

  def /(other)
    Rubinius.asm do
      flt = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt
      goto done

      flt.set!
      n_ddiv r0, r0, r1
      r_store_float r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    redo_coerced :/, other
  end

  alias_method :divide, :/
  alias_method :quo, :/
  alias_method :fdiv, :/

  INFINITY = 1.0 / 0.0
  NAN = 0.0 / 0.0

  def divmod(other)
    Rubinius.primitive :float_divmod
    b, a = math_coerce other
    a.divmod b
  end

  def %(other)
    return 0 / 0.to_f if other == 0
    Rubinius.primitive :float_mod
    b, a = math_coerce other
    a % b
  end

  alias_method :modulo, :%

  def <(other)
    Rubinius.asm do
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      n_dlt r0, r0, r1
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a < b
  end

  def <=(other)
    Rubinius.asm do
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      n_dle r0, r0, r1
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a <= b
  end

  def >(other)
    Rubinius.asm do
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      n_dgt r0, r0, r1
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a > b
  end

  def >=(other)
    Rubinius.asm do
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      n_dge r0, r0, r1
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce other, :compare_error
    a >= b
  end

  def <=>(other)
    Rubinius.primitive :float_compare
    b, a = math_coerce other, :compare_error
    a <=> b
  rescue ArgumentError
    nil
  end

  def !=(other)
    Rubinius.asm do
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      n_dne r0, r0, r1
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    b, a = math_coerce(other)
    a != b ? true : false
  end

  def ==(other)
    Rubinius.asm do
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      n_promote r2, r0, r1

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      n_deq r0, r0, r1
      r_load_bool r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    begin
      b, a = math_coerce(other)
      a == b ? true : false
    rescue TypeError
      return other == self
    end
  end

  def eql?(other)
    Rubinius.primitive :float_eql
    false
  end

  def nan?
    Rubinius.asm do
      r0 = new_register
      r1 = new_register

      r_load_self r0
      r_load_float r0, r0
      n_dnan r1, r0
      r_load_bool r0, r1
      r_ret r0

      # TODO: teach the bytecode compiler better
      push_true
    end
  end

  def infinite?
    Rubinius.asm do
      no = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register

      r_load_self r0
      r_load_float r0, r0
      n_dinf r1, r0

      r_load_0 r0
      n_ieq r2, r1, r0
      b_if r2, no

      r_store_int r0, r1
      r_ret r0

      no.set!
      r_load_nil r0, 0
      r_ret r0

      # TODO: teach the bytecode compiler better
      push_true
    end
  end

  def finite?
    not (nan? or infinite?)
  end

  def to_f
    self
  end

  def to_i
    Rubinius.primitive :float_to_i
    raise PrimitiveFailure, "Float#to_i primitive failed"
  end

  alias_method :to_int, :to_i
  alias_method :truncate, :to_i

  def to_s
    to_s_minimal
  end

  alias_method :inspect, :to_s

  def to_s_minimal
    Rubinius.primitive :float_to_s_minimal
    raise PrimitiveFailure, "Float#to_s_minimal primitive failed: output exceeds buffer size"
  end

  def to_s_formatted(fmt)
    Rubinius.primitive :float_to_s_formatted
    raise PrimitiveFailure, "Float#to_s_formatted primitive failed: output exceeds buffer size"
  end
  private :to_s_formatted

  def dtoa
    Rubinius.primitive :float_dtoa
    raise PrimitiveFailure, "Fload#dtoa primitive failed"
  end

  def to_packed(size)
    Rubinius.primitive :float_to_packed
    raise PrimitiveFailure, "Float#to_packed primitive failed"
  end

  def ceil
    int = to_i()

    return int if self == int or self < 0
    return int + 1
  end

  def floor
    int = to_i()

    return int if self > 0 or self == int
    return int - 1
  end

  def next_float
    return NAN if self.nan?
    return -MAX if self == -INFINITY
    return INFINITY if self == MAX
    return Math.ldexp(0.5, MIN_EXP - MANT_DIG + 1) if self.zero?

    frac, exp = Math.frexp self

    if frac == -0.5
      frac *= 2
      exp -= 1
    end

    smallest_frac = EPSILON / 2
    smallest_frac = Math.ldexp(smallest_frac, MIN_EXP - exp) if exp < MIN_EXP

    result_frac = frac + smallest_frac

    return -0.0 if result_frac.zero? && frac < 0
    return 0.0 if result_frac.zero? && frac > 0

    return Math.ldexp result_frac, exp
  end

  def prev_float
    return -(-self).next_float
  end
end

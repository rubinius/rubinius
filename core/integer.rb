class Integer < Numeric
  def self.===(obj)
    Rubinius.asm do
      int = new_label
      done = new_label

      r0 = new_register

      r_load_local r0, 0

      b_if_int r0, r0, int
      goto done

      int.set!
      r_load_true r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super
  end

  # unary operators

  def !
    false
  end

  def ~
    Rubinius.asm do
      eint = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_self r0

      n_promote r1, r0, r0

      r_load_2 r2
      n_ieq r3, r1, r2
      b_if r3, eint

      n_iinc r2, r2
      n_ine r3, r1, r2
      b_if r3, done

      r_load_int r0, r0
      n_inot r0, r0
      r_store_int r0, r0
      goto val

      eint.set!
      n_enot r0, r0

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super
  end

  def -@
    Rubinius.asm do
      eint = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_self r0

      n_promote r1, r0, r0

      r_load_2 r2
      n_ieq r3, r1, r2
      b_if r3, eint

      n_iinc r2, r2
      n_ine r3, r1, r2
      b_if r3, done

      n_ineg_o r0, r0
      goto val

      eint.set!
      n_eneg r0, r0

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super
  end

  def bit_length
    Rubinius.asm do
      eint = new_label
      bits = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_self r0

      n_promote r1, r0, r0

      r_load_2 r2
      n_ieq r3, r1, r2
      b_if r3, eint

      n_iinc r2, r2
      n_ine r3, r1, r2
      b_if r3, done

      r_load_int r0, r0
      n_ibits r0, r0
      r_store_int r0, r0
      goto val

      eint.set!
      r_load_0 r1
      r_store_int r1, r1
      n_promote r2, r0, r1

      r_load_2 r3
      n_ine r3, r2, r3
      b_if r3, done

      n_egt r2, r0, r1
      b_if r2, bits
      n_enot r0, r0

      bits.set!
      n_ebits r0, r0

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super
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

  def div(o)
    if o.is_a?(Float) && o == 0.0
      raise ZeroDivisionError, "division by zero"
    end

    (self / o).floor
  end

  def fdiv(other)
    to_f / other
  end

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

  def divmod(other)
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

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      r_load_0 r3
      n_ieq r3, r3, r2
      b_if r3, done

      n_edivmod r2, r0, r1
      goto val

      flt.set!
      n_ddivmod r2, r0, r1
      goto val

      int.set!
      n_idivmod r2, r0, r1

      val.set!
      r_store_stack r0
      r_store_stack r1
      make_array 2
      ret

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    redo_coerced :divmod, other
  end

  def **(o)
    Rubinius.asm do
      int = new_label
      eint = new_label
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

      r_load_2 r3
      n_ieq r3, r3, r2
      b_if r3, eint

      r_load_1 r3
      n_ieq r3, r3, r2
      b_if r3, flt

      goto done

      int.set!
      r_load_0 r2
      n_ilt r3, r1, r2
      b_if r3, done

      n_ipow_o r0, r0, r1
      goto val

      eint.set!
      r_load_0 r2
      r_store_int r2, r2
      n_promote r3, r0, r2

      n_elt r3, r1, r2
      b_if r3, done

      n_epow r0, r0, r1

      val.set!
      r_ret r0

      flt.set!
      # TODO: add n_dpow
      r_store_float r0, r0
      r_store_float r1, r1
      r_store_stack r0
      r_store_stack r1
      send :**, 1
      ret

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    if o.is_a?(Float) && self < 0 && o != o.round
      return Complex.new(self, 0) ** o
    elsif o.is_a?(Integer) && o < 0
      return Rational.new(self, 1) ** o
    end

    redo_coerced :**, o
  end

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

  def eql?(other)
    other.is_a?(Integer) && self == other
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

  def <=>(other)
    Rubinius.asm do
      int = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_2 r3
      n_ine r3, r2, r3
      b_if r3, done

      n_ecmp r0, r0, r1
      goto val

      int.set!
      n_icmp r0, r0, r1

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    if other.kind_of? Float
      return nil if other.nan?
      return -1 if other.infinite? == 1
      return 1 if other.infinite? == -1
    end

    begin
      b, a = math_coerce other, :compare_error
      return a <=> b
    rescue ArgumentError, TypeError
      return nil
    end
  end

  # bitwise binary operators

  def &(other)
    Rubinius.asm do
      int = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_1 r3
      n_iadd r3, r3, r3
      n_ine r2, r2, r3
      b_if r2, done

      n_eand r0, r0, r1
      r_ret r0

      int.set!
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
      int = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_1 r3
      n_iadd r3, r3, r3
      n_ine r2, r2, r3
      b_if r2, done

      n_eor r0, r0, r1
      r_ret r0

      int.set!
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
      int = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      n_promote r2, r0, r1

      r_load_1 r3
      n_iadd r3, r3, r3
      n_ine r2, r2, r3
      b_if r2, done

      n_exor r0, r0, r1
      r_ret r0

      int.set!
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
      shl = new_label
      neg = new_label
      eint = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, neg
      goto eint

      neg.set!
      r_load_int r2, r1
      r_load_0 r3

      n_ige r3, r2, r3
      b_if r3, shl

      n_ineg r1, r2
      r_store_int r1, r1
      n_ishr_o r0, r0, r1
      r_ret r0

      shl.set!
      n_ishl_o r0, r0, r1
      r_ret r0

      eint.set!
      n_promote r2, r0, r0
      r_load_2 r3
      n_ine r2, r2, r3
      b_if r2, done

      n_promote r2, r1, r1
      n_iinc r3, r3
      n_ine r2, r2, r3
      b_if r2, done

      r_load_int r1, r1
      n_eshl r0, r0, r1
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
      eint = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, neg
      goto eint

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

      eint.set!
      n_promote r2, r0, r0
      r_load_2 r3
      n_ine r2, r2, r3
      b_if r2, done

      n_promote r2, r1, r1
      n_iinc r3, r3
      n_ine r2, r2, r3
      b_if r2, done

      r_load_int r1, r1
      n_eshr r0, r0, r1
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

  def size
    Rubinius.asm do
      eint = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_self r0

      n_promote r1, r0, r0

      r_load_2 r2
      n_ieq r3, r1, r2
      b_if r3, eint

      n_iinc r2, r2
      n_ine r3, r1, r2
      b_if r3, done

      n_isize r0, r0
      goto val

      eint.set!
      n_esize r0, r0

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super
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

  # conversions

  def coerce(other)
    Rubinius.asm do
      val = new_label
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

      r_load_1 r3
      n_ine r3, r3, r2
      b_if r3, val

      r_store_float r0, r0
      r_store_float r1, r1

      val.set!
      r_store_stack r1
      r_store_stack r0
      make_array 2
      ret

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super other
  end

  def imaginary
    0
  end

  def to_f
    Rubinius.asm do
      eint = new_label
      done = new_label

      r0 = new_register
      r1 = new_register
      r2 = new_register
      r3 = new_register

      r_load_self r0

      n_promote r1, r0, r0

      r_load_2 r2
      n_ieq r3, r1, r2
      b_if r3, eint

      n_iinc r2, r2
      n_ine r3, r1, r2
      b_if r3, done

      n_iflt r0, r0
      r_store_float r0, r0
      r_ret r0

      eint.set!
      n_eflt r0, r0
      r_store_float r0, r0
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super
  end

  def to_s(base=10)
    Rubinius.asm do
      int = new_label
      eint = new_label
      base = new_label
      val = new_label
      done = new_label

      r0 = new_register
      r1 = new_register

      r_load_m_binops r0, r1

      b_if_int r0, r1, int

      b_if_eint r0, r0, base
      goto done

      base.set!
      b_if_int r1, r1, eint
      goto done

      eint.set!
      n_estr r0, r0, r1
      goto val

      int.set!
      n_istr r0, r0, r1

      val.set!
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super base
  end

  # We do not alias this to #to_s in case someone overrides #to_s.
  def inspect
    Rubinius.asm do
      int = new_label
      eint = new_label
      done = new_label

      r0 = new_register
      r1 = new_register

      r_load_literal r1, 10

      r_load_self r0

      b_if_int r0, r0, int
      b_if_eint r0, r0, eint
      goto done

      int.set!
      n_istr r0, r0, r1
      r_ret r0

      eint.set!
      n_estr r0, r0, r1
      r_ret r0

      done.set!

      # TODO: teach the bytecode compiler better
      push_true
    end

    super
  end

  # predicates

  def zero?
    self == 0
  end

end

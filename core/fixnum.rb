class Fixnum < Integer
  def self.===(obj)
    Rubinius.asm do
      int = new_label
      done = new_label

      r0 = new_register
      r1 = new_register

      r_load_m_binops r0, r1

      b_if_int r1, r1, int
      r_load_false r0
      goto done

      int.set!
      r_load_true r0

      done.set!
      r_ret r0

      # TODO: teach the bytecode compiler better
      push_true
    end
  end

  def <=>(other)
    Rubinius.primitive :fixnum_compare

    # DO NOT super to Numeric#<=>. It does not contain the coerce
    # protocol.

    begin
      b, a = math_coerce(other, :compare_error)
      return a <=> b
    rescue ArgumentError
      return nil
    end
  end
end

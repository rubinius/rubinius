class Fixnum < Integer
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

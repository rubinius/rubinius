##
# Bignum objects hold integers outside the range of Fixnum. Bignum objects are
# created automatically when integer calculations would otherwise overflow a
# Fixnum. When a calculation involving Bignum objects returns a result that
# will fit in a Fixnum, the result is automatically converted.
#
# For the purposes of the bitwise operations and [], a Bignum is treated as if
# it were an infinite-length bitstring with 2's complement representation.
#
# While Fixnum values are immediate, Bignum objects are not. Assignment and
# parameter passing work with references to objects, not the objects
# themselves.

class Bignum < Integer

  # see README-DEVELOPERS regarding safe math compiler plugin
  alias_method :/, :divide

  def %(other)
    if other.kind_of?(Float)
      if other == 0.0
        return 0 / 0.0
      else
        return self.to_f % other
      end
    end

    raise TypeError unless other.kind_of?(Numeric)
    raise ZeroDivisionError if other == 0

    if self == 0 || self.abs == other.abs
      0
    else
      mod_primitive(other)#.to_int)
    end
  end

  def eql?(value)
    value.is_a?(Bignum) && self == value
  end

  alias_method :modulo, :%

  def fdiv(n)
    to_f / n
  end

  alias_method :quo, :fdiv
end

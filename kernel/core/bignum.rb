# depends on: integer.rb

class Bignum < Integer
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

  def remainder(other)
    a = self
    b = other

    mod = a % b

    # unlike Numeric#remainder, we just return the result if it's zero
    if mod != 0 && (a < 0 && b > 0 || a > 0 && b < 0)
      mod - b
    else
      mod
    end
  end

  def >>(s)
    s = Type.coerce_to(s, Fixnum, :to_int)
    s < 0 ? __bignum_left_shift__(-s) : __bignum_right_shift__(s) 
  end

  def <<(s)
    s = Type.coerce_to(s, Fixnum, :to_int)
    s < 0 ? __bignum_right_shift__(-s) : __bignum_left_shift__(s) 
  end

  def eql?(value)
    return false unless value.kind_of?(Integer)

    self == value
  end

  alias_method :modulo, :%
  
  alias_method :div, :/
end

# depends on: integer.rb

class Bignum < Integer
  def %(other)
    if Float === other && other == 0.0
      0 / 0.0
    else
      self.divmod(other)[1]
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
  
  def coerce(other)
    return super(other) if other.is_a?(Integer)
    raise TypeError, "can't coerce #{other.class} to Bignum"
  end

  alias_method :modulo, :%
  
  alias_method :div, :/
end

# depends on: integer.rb

class Fixnum < Integer
  
  MAX = Platform::Fixnum.MAX
  
  alias_method :modulo, :%
  
  def <<(c)
    c = Type.coerce_to(c, Fixnum, :to_int)
    return self >> -c if c < 0

    bits = self.size * 8 - 1
    if (c > bits || self >> bits - c > 0 || c.kind_of?(Bignum))
      return __bignum_new__(self) << c
    end
    __fixnum_left_shift__(c)
  end

  def >>(c)
    c = Type.coerce_to(c, Fixnum, :to_int)
    return self << -c if c < 0
    
    if c.kind_of?(Bignum)
      return __bignum_new__(self) >> c
    end

    __fixnum_right_shift__(c)
  end
  
  def id2name
    Symbols.symbols.to_a[self]
  end

  def to_sym
    id2name.to_sym
  rescue NoMethodError
    nil
  end
end
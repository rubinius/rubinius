# depends on: integer.rb class.rb

##
#--
# NOTE do not define to_sym or id2name. It's been deprecated for 5 years and
# we've decided to remove it.
#++

class Fixnum < Integer

  MAX = Platform::Fixnum.MAX

  def self.induced_from(obj)
    case obj
    when Fixnum
      return obj
    when Float, Bignum
      value = obj.to_i
      if value.is_a? Bignum
        raise RangeError, "Object is out of range for a Fixnum"
      else
        return value
      end
    else
      value = Type.coerce_to(obj, Integer, :to_int)
      return self.induced_from(value)
    end
  end

  #--
  # see README-DEVELOPERS regarding safe math compiler plugin
  #++

  alias_method :/, :divide
  alias_method :modulo, :%
  
  def <<(c)
    c = Type.coerce_to(c, Integer, :to_int)
    raise RangeError, "Object is out of range for a Fixnum" unless c.is_a?(Fixnum)
    return self >> -c if c < 0

    bits = self.size * 8 - 1
    if (c > bits || self >> bits - c > 0 || c.kind_of?(Bignum))
      return __bignum_new__(self) << c
    end
    __fixnum_left_shift__(c)
  end

  def >>(c)
    c = Type.coerce_to(c, Integer, :to_int)
    return self << -c if c < 0
    if c > self.abs
      return  0 if self >= 0
      return -1 if self <  0
    end
    
    if c.kind_of?(Bignum)
      return __bignum_new__(self) >> c
    end

    __fixnum_right_shift__(c)
  end
  
  def to_s(base=10)
    raise ArgumentError, 'base must be between 2 and 36' unless base.between?(2, 36)
    based_to_s(base)
  end
  private :base_to_s

  # taint and untaint are noops on Fixnum
  def taint
    self
  end
  
  def untaint
    self
  end
end

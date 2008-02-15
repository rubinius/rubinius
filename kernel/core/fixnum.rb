# depends on: integer.rb class.rb

##
#--
# NOTE do not define to_sym or id2name. It's been deprecated for 5 years and
# we've decided to remove it.
#++

class Fixnum < Integer

  MAX = Platform::Fixnum.MAX

  #--
  # see README-DEVELOPERS regarding safe math compiler plugin
  #++

  alias_method :/, :divide
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
  
  def to_s(base=10)
    raise ArgumentError, 'base must be between 2 and 36' unless base.between?(2, 36)
    based_to_s(base)
  end
  private :base_to_s
 
  def b64_symbol_value
    if self >= 65 and self <= 90  # A-Z
      self - 65
    elsif self >= 97 and self <= 122  # a-z
      self - 71
    elsif self >= 48 and self <= 57  # 0-9
      self + 4
    elsif self == 43  # +
      62
    elsif self == 47  # /
      63
    elsif self == 61  # =
      0
    else
      nil
    end
  end
end

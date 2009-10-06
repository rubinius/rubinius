##
#--
# NOTE do not define to_sym or id2name. It's been deprecated for 5 years and
# we've decided to remove it.
#++

class Fixnum < Integer

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

  def fdiv(n)
    to_f / n
  end

  alias_method :quo, :fdiv

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

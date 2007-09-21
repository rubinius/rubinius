# depends on: numeric.rb

class Float < Numeric
  
  def __ivars__; nil; end
  
  RADIX      = Platform::Float.RADIX
  ROUNDS     = Platform::Float.ROUNDS
  MIN        = Platform::Float.MIN
  MAX        = Platform::Float.MAX
  MIN_EXP    = Platform::Float.MIN_EXP
  MAX_EXP    = Platform::Float.MAX_EXP
  MIN_10_EXP = Platform::Float.MIN_10_EXP
  MAX_10_EXP = Platform::Float.MAX_10_EXP
  DIG        = Platform::Float.DIG
  MANT_DIG   = Platform::Float.MANT_DIG
  EPSILON    = Platform::Float.EPSILON
  
  def self.induced_from(obj)
    if [Float, Bignum, Fixnum].include?(obj.class)
      obj.to_f
    else
      raise TypeError, "failed to convert #{obj.class} into Float"
    end
  end
  
  def +(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.add self, other
  end
  
  def -(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.sub self, other
  end
  
  def *(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.mul self, other
  end
  
  def /(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.div self, other
  end
  
  def -@
    Platform::Float.uminus self
  end

  alias_method :inspect, :to_s
  
  def to_i
    if infinite?
      raise FloatDomainError, "Infinity"
    elsif nan?
      return self
    else
      Platform::Float.to_i self
    end
  end

  def finite?
    not (nan? or infinite?) 
  end

  def to_f
    self
  end
    
  alias_method :to_int, :to_i
  alias_method :truncate, :to_i
  
  def %(other)
    return 0 / 0.to_f if other == 0
    self.divmod(Float(other))[1]
  end
  
  alias_method :modulo, :%
  
end


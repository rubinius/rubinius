# depends on: numeric.rb precision.rb class.rb

class Float < Numeric

  include Precision

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
  STRLEN     = 32
  
  alias_method :quo, :/
  
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
  
  # see README-DEVELOPERS regarding safe math compiler plugin
  def divide(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.div self, other
  end
  alias_method :/, :divide
  
  def -@
    Platform::Float.uminus self
  end

  def <=>(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.compare self, other
  end

  def ==(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.eql? self, other
  end

  def eql?(other)
    return false unless other.is_a?(Float)
    Platform::Float.eql? self, other
  end
  
  def divmod(other)
    raise FloatDomainError, "divide by 0" if other == 0
    return super(other) unless other.is_a?(Float)
    div = (self / other).floor;
    mod = Platform::Float.fmod self, other
    
    if (other * mod < 0)
    	mod += other;
    end
    return [div.to_i, mod]
  end

  def nan?
    Platform::Float.nan? self
  end

  def infinite?
    Platform::Float.infinite? self
  end
  
  def finite?
    not (nan? or infinite?) 
  end
  
  def **(other)
    return super(other) unless other.is_a?(Float)
    Platform::Float.pow self, other
  end
  
  def to_f
    self
  end
  
  def to_i
    if infinite?
      raise FloatDomainError, self < 0 ? "-Infinity" : "Infinity"
    elsif nan?
      return self
    else
      if self < Platform::Fixnum.MAX.to_f && self > Platform::Fixnum.MIN.to_f
        Platform::Float.to_i self
      else
        Bignum.from_float self
      end
    end
  end
  alias_method :to_int, :to_i
  alias_method :truncate, :to_i
  
  def to_s
    return (self < 0 ? "-Infinity" : "Infinity") if infinite?
    return "NaN" if nan?
    
    str = Platform::Float.to_s_formatted STRLEN, "%#.15g", self
    e = str.index('e') || str.size
    unless str[e-1].isdigit
      str = Platform::Float.to_s_formatted STRLEN, "%#.14e", self
      e = str.index('e') || str.size
    end
    str.gsub(/(\.\d+?)(0+)($|e[+-]\d*)/, '\1\3')
  end
  alias_method :inspect, :to_s

  def to_s_formatted(fmt)
    Platform::Float.to_s_formatted STRLEN, fmt, self
  end
  
  def %(other)
    return 0 / 0.to_f if other == 0
    self.divmod(Float(other))[1]
  end
  alias_method :modulo, :%
  
  def round
    if self < Platform::Fixnum.MAX.to_f && self > Platform::Fixnum.MIN.to_f
      Platform::Float.round self
    else
      Bignum.from_float self
    end
  end
  
end


# depends on: numeric.rb

class Float < Numeric
  
  def __ivars__; nil; end
  
  def self.induced_from(obj)
    if [Float, Bignum, Fixnum].include?(obj.class)
      obj.to_f
    else
      raise TypeError, "failed to convert #{obj.class} into Float"
    end
  end

  alias_method :inspect, :to_s

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


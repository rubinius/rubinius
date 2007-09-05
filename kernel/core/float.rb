# See rake build:core
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

  alias :inspect :to_s

  def finite?
    not (nan? or infinite?) 
  end

  def to_f
    self
  end
    
  alias :to_int :to_i
  alias :truncate :to_i
  
  def %(other)
    return 0 / 0.to_f if other == 0
    self.divmod(Float(other))[1]
  end
  
  alias :modulo :%

end


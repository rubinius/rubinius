module Precision

  def prec(klass)
    klass.induced_from(self)
  end

  def prec_f
    prec(Float)
  end

  def prec_i
    prec(Integer)
  end
  
end
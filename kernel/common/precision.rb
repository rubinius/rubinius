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

  def self.included(klass)
    def klass.induced_from(obj)
      raise TypeError
    end
  end
end

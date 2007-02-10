
module Math

  # Constants
# FIXME:
#  PI=3.14159_26535_89793_23846
#  E=2.71828_18284_59045_23536

# FIXME: extend Math

  class << self
    include Math
  end

  def sqrt(v)
    Ruby.primitive :math_sqrt
    sqrt(Float(v))
  end


  
end

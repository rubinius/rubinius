# class Float < Numeric
#   include Precision
#   
#   def radix_to_s(radix)
#     Ruby.primitive :float_to_s
#   end
# 
#   def to_s(radix=10)
#     radix_to_s(radix)
#   end
# 
#   def inspect
#     radix_to_s(10)
#   end
#   
#   def +(o)
#     Ruby.primitive :float_add
#   end
# 
#   def -(o)
#     Ruby.primitive :float_sub
#   end
# 
#   def *(o)
#     Ruby.primitive :float_mul
#   end
# 
#   def ==(o)
#     Ruby.primitive :float_equal
#   end
# end

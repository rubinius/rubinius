# depends on: ffi.rb

class Platform::Float
  attach_function nil, 'float_radix',      :RADIX, [], :int
  attach_function nil, 'float_rounds',     :ROUNDS, [], :int
  attach_function nil, 'float_min',        :MIN, [], :double
  attach_function nil, 'float_max',        :MAX, [], :double
  attach_function nil, 'float_min_exp',    :MIN_EXP, [], :int
  attach_function nil, 'float_max_exp',    :MAX_EXP, [], :int
  attach_function nil, 'float_min_10_exp', :MIN_10_EXP, [], :int
  attach_function nil, 'float_max_10_exp', :MAX_10_EXP, [], :int
  attach_function nil, 'float_dig',        :DIG, [], :int
  attach_function nil, 'float_mant_dig',   :MANT_DIG, [], :int
  attach_function nil, 'float_epsilon',    :EPSILON, [], :double
  
  attach_function nil, 'float_to_i', :to_i, [:double], :int
  attach_function nil, 'float_add',  :add, [:double, :double], :double
  attach_function nil, 'float_sub',  :sub, [:double, :double], :double
  attach_function nil, 'float_mul',  :mul, [:double, :double], :double
  attach_function nil, 'float_div',  :div, [:double, :double], :double
  attach_function nil, 'float_uminus',  :uminus,      [:double], :double
  attach_function nil, 'float_equal',   :value_equal, [:double, :double], :int
  attach_function nil, 'float_compare', :compare,     [:double, :double], :int
  attach_function nil, 'float_round',   :round,       [:double], :int
  attach_function nil, 'float_sprintf', :sprintf,     [:pointer, :int, :string, :double], :int
  attach_function nil, 'fmod',  [:double, :double], :double
  attach_function nil, 'pow',   [:double, :double], :double
  attach_function nil, 'isnan', [:double], :int
  attach_function nil, 'isinf', [:double], :int
  
  def self.eql?(a, b)
    value_equal(a, b) == 1
  end
  
  def self.nan?(value)
    isnan(value) == 1
  end
  
  def self.infinite?(value)
    return (value < 0 ? -1 : 1) if isinf(value) == 1
  end
end

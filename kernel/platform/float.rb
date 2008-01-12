# depends on: ffi.rb

class Platform::Float
  attach_function 'float_radix',      :RADIX, [], :int
  attach_function 'float_rounds',     :ROUNDS, [], :int
  attach_function 'float_min',        :MIN, [], :double
  attach_function 'float_max',        :MAX, [], :double
  attach_function 'float_min_exp',    :MIN_EXP, [], :int
  attach_function 'float_max_exp',    :MAX_EXP, [], :int
  attach_function 'float_min_10_exp', :MIN_10_EXP, [], :int
  attach_function 'float_max_10_exp', :MAX_10_EXP, [], :int
  attach_function 'float_dig',        :DIG, [], :int
  attach_function 'float_mant_dig',   :MANT_DIG, [], :int
  attach_function 'float_epsilon',    :EPSILON, [], :double

  attach_function 'float_to_i', :to_i, [:double], :int
  attach_function 'float_add',  :add, [:double, :double], :double
  attach_function 'float_sub',  :sub, [:double, :double], :double
  attach_function 'float_mul',  :mul, [:double, :double], :double
  attach_function 'float_div',  :div, [:double, :double], :double
  attach_function 'float_uminus',  :uminus,      [:double], :double
  attach_function 'float_equal',   :value_equal, [:double, :double], :int
  attach_function 'float_compare', :compare,     [:double, :double], :int
  attach_function 'float_round',   :round,       [:double], :int
  attach_function 'fmod',  [:double, :double], :double
  attach_function 'pow',   [:double, :double], :double
  attach_function 'isnan', [:double], :int
  attach_function 'isinf', [:double], :int

  attach_function 'frexp', [:double, :pointer], :double
  attach_function 'ldexp', [:double, :int], :double
  attach_function 'modf', [:double, :pointer], :double

  def self.eql?(a, b)
    value_equal(a, b) == 1
  end
  
  def self.nan?(value)
    isnan(value) == 1
  end
  
  def self.infinite?(value)
    return (value < 0 ? -1 : 1) if isinf(value) != 0
  end
  
  def self.to_s_formatted(size, fmt, value)
    s, p = Platform::POSIX.sprintf_f value, size, fmt
    str = s.dup
    p.free
    return str
  end
end

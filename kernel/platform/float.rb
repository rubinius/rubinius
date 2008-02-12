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

  attach_function 'frexp', [:double, :pointer], :double
  attach_function 'ldexp', [:double, :int], :double
  attach_function 'modf', [:double, :pointer], :double

  def self.to_s_formatted(size, fmt, value)
    s, p = Platform::POSIX.sprintf_f value, size, fmt
    str = s.dup
    p.free
    return str
  end
end

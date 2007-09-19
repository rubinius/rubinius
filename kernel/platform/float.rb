class Platform::Float
  attach_function nil, 'ffi_float_radix',      :RADIX, [], :int
  attach_function nil, 'ffi_float_rounds',     :ROUNDS, [], :int
  attach_function nil, 'ffi_float_min',        :MIN, [], :double
  attach_function nil, 'ffi_float_max',        :MAX, [], :double
  attach_function nil, 'ffi_float_min_exp',    :MIN_EXP, [], :int
  attach_function nil, 'ffi_float_max_exp',    :MAX_EXP, [], :int
  attach_function nil, 'ffi_float_min_10_exp', :MIN_10_EXP, [], :int
  attach_function nil, 'ffi_float_max_10_exp', :MAX_10_EXP, [], :int
  attach_function nil, 'ffi_float_dig',        :DIG, [], :int
  attach_function nil, 'ffi_float_mant_dig',   :MANT_DIG, [], :int
  attach_function nil, 'ffi_float_epsilon',    :EPSILON, [], :double
  
  attach_function nil, 'ffi_float_to_i', :to_i, [:double], :int
end

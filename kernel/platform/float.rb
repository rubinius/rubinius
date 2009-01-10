# depends on: library.rb

##
# Platform specific behavior for Float.

class Platform::Float
  extend FFI::Library

  # HACK these should be primitives, not FFI
  attach_function :RADIX,      'float_radix',      [], :int
  attach_function :ROUNDS,     'float_rounds',     [], :int
  attach_function :MIN,        'float_min',        [], :double
  attach_function :MAX,        'float_max',        [], :double
  attach_function :MIN_EXP,    'float_min_exp',    [], :int
  attach_function :MAX_EXP,    'float_max_exp',    [], :int
  attach_function :MIN_10_EXP, 'float_min_10_exp', [], :int
  attach_function :MAX_10_EXP, 'float_max_10_exp', [], :int
  attach_function :DIG,        'float_dig',        [], :int
  attach_function :MANT_DIG,   'float_mant_dig',   [], :int
  attach_function :EPSILON,    'float_epsilon',    [], :double

  attach_function :frexp, [:double, :pointer], :double
  attach_function :ldexp, [:double, :int],     :double
  attach_function :modf,  [:double, :pointer], :double
end


# depends on: ffi.rb

module Platform::Math
  attach_function nil, "atan2", [:double, :double], :double
  attach_function nil, "cos",   [:double], :double
  attach_function nil, "sin",   [:double], :double
  attach_function nil, "tan",   [:double], :double
  attach_function nil, "acos",  [:double], :double
  attach_function nil, "asin",  [:double], :double
  attach_function nil, "atan",  [:double], :double
  attach_function nil, "cosh",  [:double], :double
  attach_function nil, "sinh",  [:double], :double
  attach_function nil, "tanh",  [:double], :double
  attach_function nil, "acosh", [:double], :double
  attach_function nil, "asinh", [:double], :double
  attach_function nil, "atanh", [:double], :double
  attach_function nil, "exp",   [:double], :double
  attach_function nil, "log",   [:double], :double
  attach_function nil, "log2",  [:double], :double
  attach_function nil, "log10", [:double], :double
  attach_function nil, "sqrt",  [:double], :double
  attach_function nil, "frexp", [:double, :pointer], :double
  attach_function nil, "ldexp", [:double, :int], :double
  attach_function nil, "hypot", [:double, :double], :double
  attach_function nil, "erf",   [:double], :double
  attach_function nil, "erfc",  [:double], :double
end

# depends on: ffi.rb

module Platform::Math
  attach_function nil, "sqrt", [:double], :double
end

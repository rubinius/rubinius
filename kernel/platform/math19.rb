# -*- encoding: us-ascii -*-

##
# Platform specific behavior for Math in 1.9.

module Rubinius
module FFI::Platform::Math
  attach_function :cbrt, [:double], :double
  attach_function :tgamma, [:double], :double
  attach_function :lgamma_r, [:double, :pointer], :double
end
end

# -*- encoding: us-ascii -*-

##
# Platform specific behavior for Math.

module Rubinius
module FFI::Platform::Math
  extend FFI::Library

  attach_function :fabs,  [:double], :double
  attach_function :atan2, [:double, :double], :double
  attach_function :cos,   [:double], :double
  attach_function :sin,   [:double], :double
  attach_function :tan,   [:double], :double
  attach_function :acos,  [:double], :double
  attach_function :asin,  [:double], :double
  attach_function :atan,  [:double], :double
  attach_function :cosh,  [:double], :double
  attach_function :sinh,  [:double], :double
  attach_function :tanh,  [:double], :double
  attach_function :acosh, [:double], :double
  attach_function :asinh, [:double], :double
  attach_function :atanh, [:double], :double
  attach_function :exp,   [:double], :double
  attach_function :log,   [:double], :double
  attach_function :log10, [:double], :double
  attach_function :sqrt,  [:double], :double
  attach_function :frexp, [:double, :pointer], :double
  attach_function :ldexp, [:double, :int], :double
  attach_function :hypot, [:double, :double], :double
  attach_function :erf,   [:double], :double
  attach_function :erfc,  [:double], :double

  # Rubinius-specific, used in Marshal
  attach_function :modf,  [:double, :pointer], :double

  # TODO: we need a way to determine whether a function
  # is defined for a particular platform.
  #
  # attach_function :log2,  [:double], :double
  def self.log2(x)
    log10(x) / log10(2.0)
  end
end
end

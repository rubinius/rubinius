# -*- encoding: us-ascii -*-

module Math
  FFI = Rubinius::FFI

  # Constants
  PI = 3.14159_26535_89793_23846
  E  = 2.71828_18284_59045_23536

  if Errno.const_defined? :EDOM
    DomainError = Errno::EDOM
  elsif Errno.const_defined? :ERANGE
    DomainError = Errno::ERANGE
  else
    class DomainError < SystemCallError
    end
  end

  def atan2(y, x)
    y = Rubinius::Type.coerce_to_float y
    x = Rubinius::Type.coerce_to_float x
    FFI::Platform::Math.atan2 y, x
  end

  def cos(x)
    FFI::Platform::Math.cos Rubinius::Type.coerce_to_float(x)
  end

  def sin(x)
    FFI::Platform::Math.sin Rubinius::Type.coerce_to_float(x)
  end

  def tan(x)
    FFI::Platform::Math.tan Rubinius::Type.coerce_to_float(x)
  end

  def atan(x)
    FFI::Platform::Math.atan Rubinius::Type.coerce_to_float(x)
  end

  def cosh(x)
    FFI::Platform::Math.cosh Rubinius::Type.coerce_to_float(x)
  end

  def sinh(x)
    FFI::Platform::Math.sinh Rubinius::Type.coerce_to_float(x)
  end

  def tanh(x)
    FFI::Platform::Math.tanh Rubinius::Type.coerce_to_float(x)
  end

  def asinh(x)
    FFI::Platform::Math.asinh Rubinius::Type.coerce_to_float(x)
  end

  # This is wierd, but we need to only do the ERANGE check if
  # there is an ERANGE.
  if Errno.const_defined?(:ERANGE)

    def atanh(x)
      x = Rubinius::Type.coerce_to_float(x)
      raise DomainError, 'atanh' unless x.abs <= 1.0

      FFI::Platform::POSIX.errno = 0

      ret = FFI::Platform::Math.atanh x
      begin
        Errno.handle

        # Linux sets errno to ERANGE, but it should be EDOM
      rescue Errno::ERANGE
        raise Errno::EDOM
      end

      ret
    end

  else

    def atanh(x)
      x = Rubinius::Type.coerce_to_float(x)
      raise DomainError, 'atanh' unless x.abs <= 1.0

      FFI::Platform::POSIX.errno = 0

      ret = FFI::Platform::Math.atanh x
      Errno.handle
      ret
    end

  end

  def exp(x)
    FFI::Platform::Math.exp Rubinius::Type.coerce_to_float(x)
  end

  def frexp(x)
    x = Rubinius::Type.coerce_to_float(x)
    FFI::MemoryPointer.new :int do |exp|
      result = FFI::Platform::Math.frexp x, exp
      [result, exp.read_int]
    end
  end

  def ldexp(x, n)
    if n.kind_of? Float and n.nan?
      raise RangeError, "NaN cannot be converted to an Integer"
    end

    n = Rubinius::Type.coerce_to(n, Integer, :to_int)

    FFI::Platform::Math.ldexp Rubinius::Type.coerce_to_float(x), n
  end

  # Rubinius-specific, used in Marshal
  def modf(x)
    FFI::MemoryPointer.new :double do |integral|
      fractional = FFI::Platform::Math.modf x, integral
      [fractional, integral.read_double]
    end
  end

  def hypot(x, y)
    x = Rubinius::Type.coerce_to_float x
    y = Rubinius::Type.coerce_to_float y
    FFI::Platform::Math.hypot x, y
  end

  def erf(x)
    FFI::Platform::Math.erf Rubinius::Type.coerce_to_float(x)
  end

  def erfc(x)
    FFI::Platform::Math.erfc Rubinius::Type.coerce_to_float(x)
  end
end

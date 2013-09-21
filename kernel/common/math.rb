module Math
  FFI = Rubinius::FFI

  # Constants
  PI = 3.14159_26535_89793_23846
  E  = 2.71828_18284_59045_23536

  FactorialTable = [
    1.0,
    1.0,
    2.0,
    6.0,
    24.0,
    120.0,
    720.0,
    5040.0,
    40320.0,
    362880.0,
    3628800.0,
    39916800.0,
    479001600.0,
    6227020800.0,
    87178291200.0,
    1307674368000.0,
    20922789888000.0,
    355687428096000.0,
    6402373705728000.0,
    121645100408832000.0,
    2432902008176640000.0,
    51090942171709440000.0,
    1124000727777607680000.0
  ]

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

  def asin(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'asin' unless x.abs <= 1.0
    FFI::Platform::Math.asin x
  end

  def acos(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'acos' unless x.abs <= 1.0

    FFI::Platform::POSIX.errno = 0

    ret = FFI::Platform::Math.acos x
    Errno.handle
    ret
  end

  def acosh(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'acosh' unless x >= 1.0
    FFI::Platform::Math.acosh x
  end

  def cbrt(x)
    x = Rubinius::Type.coerce_to_float x
    FFI::Platform::Math.cbrt x
  end

  def gamma(x)
    x = Rubinius::Type.coerce_to_float x

    # if x is negative zero, return -infinity
    return -Float::INFINITY if (1 / x) == -Float::INFINITY
    return Float::INFINITY if x == 0.0
    return Float::NAN if x.nan?

    if sign = x.infinite?
      raise DomainError, "gamma" if sign == -1

      return Float::INFINITY
    end

    FFI::MemoryPointer.new :double do |integral|
      fractional = FFI::Platform::Math.modf x, integral
      int = integral.read_double.to_i

      if fractional == 0.0
        raise DomainError, "gamma" if int < 0
        return FactorialTable[int - 1] if int <= FactorialTable.size
      end
    end

    FFI::Platform::Math.tgamma x
  end

  def lgamma(x)
    x = Rubinius::Type.coerce_to_float x

    if sign = x.infinite?
      raise DomainError, "lgamma" if sign == -1

      return [Float::INFINITY, 1]
    end

    FFI::MemoryPointer.new :int do |sign|
      sign.write_int 1
      result = FFI::Platform::Math.lgamma_r x, sign
      [result, sign.read_int]
    end
  end

  def log(x, base=undefined)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float x
    raise DomainError, 'log' unless x >= 0.0
    return -Float::INFINITY if x == 0.0
    y = FFI::Platform::Math.log x
    unless undefined.equal? base
      base = Rubinius::Type.coerce_to_float base
      y /= log(base)
    end
    y
  end

  def log2(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'log2' unless x >= 0.0
    FFI::Platform::Math.log2 x
  end

  def log10(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'log10' unless x >= 0.0
    FFI::Platform::Math.log10 x
  end

  def sqrt(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'sqrt' unless x >= 0.0
    FFI::Platform::Math.sqrt x
  end
end

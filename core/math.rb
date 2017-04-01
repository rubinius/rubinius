module Math
  def atan2(y, x)
    y = Rubinius::Type.coerce_to_float y
    x = Rubinius::Type.coerce_to_float x
    FFI::Platform::Math.atan2 y, x
  end
  module_function :atan2

  def cos(x)
    FFI::Platform::Math.cos Rubinius::Type.coerce_to_float(x)
  end
  module_function :cos

  def sin(x)
    FFI::Platform::Math.sin Rubinius::Type.coerce_to_float(x)
  end
  module_function :sin

  def tan(x)
    FFI::Platform::Math.tan Rubinius::Type.coerce_to_float(x)
  end
  module_function :tan

  def atan(x)
    FFI::Platform::Math.atan Rubinius::Type.coerce_to_float(x)
  end
  module_function :atan

  def cosh(x)
    FFI::Platform::Math.cosh Rubinius::Type.coerce_to_float(x)
  end
  module_function :cosh

  def sinh(x)
    FFI::Platform::Math.sinh Rubinius::Type.coerce_to_float(x)
  end
  module_function :sinh

  def tanh(x)
    FFI::Platform::Math.tanh Rubinius::Type.coerce_to_float(x)
  end
  module_function :tanh

  def asinh(x)
    FFI::Platform::Math.asinh Rubinius::Type.coerce_to_float(x)
  end
  module_function :asinh

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
  module_function :atanh

  def exp(x)
    FFI::Platform::Math.exp Rubinius::Type.coerce_to_float(x)
  end
  module_function :exp

  def frexp(x)
    x = Rubinius::Type.coerce_to_float(x)
    FFI::MemoryPointer.new :int do |exp|
      result = FFI::Platform::Math.frexp x, exp
      [result, exp.read_int]
    end
  end
  module_function :frexp

  def ldexp(x, n)
    if n.kind_of? Float and n.nan?
      raise RangeError, "NaN cannot be converted to an Integer"
    end

    n = Rubinius::Type.coerce_to(n, Integer, :to_int)

    FFI::Platform::Math.ldexp Rubinius::Type.coerce_to_float(x), n
  end
  module_function :ldexp

  # Rubinius-specific, used in Marshal
  def modf(x)
    FFI::MemoryPointer.new :double do |integral|
      fractional = FFI::Platform::Math.modf x, integral
      [fractional, integral.read_double]
    end
  end
  module_function :modf

  def hypot(x, y)
    x = Rubinius::Type.coerce_to_float x
    y = Rubinius::Type.coerce_to_float y
    FFI::Platform::Math.hypot x, y
  end
  module_function :hypot

  def erf(x)
    FFI::Platform::Math.erf Rubinius::Type.coerce_to_float(x)
  end
  module_function :erf

  def erfc(x)
    FFI::Platform::Math.erfc Rubinius::Type.coerce_to_float(x)
  end
  module_function :erfc

  def asin(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'asin' unless x.abs <= 1.0
    FFI::Platform::Math.asin x
  end
  module_function :asin

  def acos(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'acos' unless x.abs <= 1.0

    FFI::Platform::POSIX.errno = 0

    ret = FFI::Platform::Math.acos x
    Errno.handle
    ret
  end
  module_function :acos

  def acosh(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'acosh' unless x >= 1.0
    FFI::Platform::Math.acosh x
  end
  module_function :acosh

  def cbrt(x)
    x = Rubinius::Type.coerce_to_float x
    FFI::Platform::Math.cbrt x
  end
  module_function :cbrt

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
  module_function :gamma

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
  module_function :lgamma

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
  module_function :log

  def log2(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    if x.kind_of? Bignum and x >= 0 and Float::MAX_EXP <= (bits = x.bit_length)
      bits -= Float::MANT_DIG
      x >>= bits
    else
      bits = 0
    end

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'log2' unless x >= 0.0
    FFI::Platform::Math.log2(x) + bits
  end
  module_function :log2

  def log10(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'log10' unless x >= 0.0
    FFI::Platform::Math.log10 x
  end
  module_function :log10

  def sqrt(x)
    return Float::NAN if x.kind_of? Float and x.nan?

    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'sqrt' unless x >= 0.0
    FFI::Platform::Math.sqrt x
  end
  module_function :sqrt
end

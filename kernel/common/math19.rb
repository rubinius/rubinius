# -*- encoding: us-ascii -*-

module Math
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

  def cbrt(x)
    x = Rubinius::Type.coerce_to_float x
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
      result = FFI::Platform::Math.lgamma_r x, sign
      [result, sign.read_int]
    end
  end

  def log(x, base=undefined)
    x = Rubinius::Type.coerce_to_float x
    raise DomainError, 'log' unless x >= 0.0
    return -Float::INFINITY if x == 0.0
    y = FFI::Platform::Math.log x
    unless base.equal? undefined
      base = Rubinius::Type.coerce_to_float base
      y /= log(base)
    end
    y
  end
end

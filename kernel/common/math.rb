module Math
  # Constants
  PI = 3.14159_26535_89793_23846
  E  = 2.71828_18284_59045_23536

  def atan2(y, x)
    Platform::Math.atan2 Float(y), Float(x)
  end

  def cos(x)
    Platform::Math.cos Float(x)
  end

  def sin(x)
    Platform::Math.sin Float(x)
  end

  def tan(x)
    Platform::Math.tan Float(x)
  end

  def acos(x)
    x = Float(x)
    verify_domain('acos') { x.abs <= 1.0 }

    Platform::POSIX.errno = 0

    ret = Platform::Math.acos x
    Errno.handle
    ret
  end

  def asin(x)
    x = Float(x)
    verify_domain('asin') { x.abs <= 1.0 }
    Platform::Math.asin x
  end

  def atan(x)
    Platform::Math.atan Float(x)
  end

  def cosh(x)
    Platform::Math.cosh Float(x)
  end

  def sinh(x)
    Platform::Math.sinh Float(x)
  end

  def tanh(x)
    Platform::Math.tanh Float(x)
  end

  def acosh(x)
    x = Float(x)
    verify_domain('acosh') { x >= 1.0 }
    Platform::Math.acosh x
  end

  def asinh(x)
    Platform::Math.asinh Float(x)
  end

  def atanh(x)
    x = Float(x)
    verify_domain('atanh') { x.abs <= 1.0 }

    Platform::POSIX.errno = 0

    ret = Platform::Math.atanh x
    Errno.handle
    ret
  end

  def exp(x)
    Platform::Math.exp Float(x)
  end

  def log(x, base=nil)
    x = Float(x)
    verify_domain('log') { x >= 0.0 }
    result = Platform::Math.log x
    result /= Platorm::Math.log Float(base) if base
    return result
  end

  def log2(x)
    x = Float(x)
    verify_domain('log2') { x >= 0.0 }
    Platform::Math.log2 x
  end

  def log10(x)
    x = Float(x)
    verify_domain('log10') { x >= 0.0 }
    Platform::Math.log10 x
  end

  def sqrt(x)
    x = Float(x)
    verify_domain('sqrt') { x >= 0.0 }
    Platform::Math.sqrt x
  end

  def frexp(x)
    x = Float(x)
    FFI::MemoryPointer.new :int do |exp|
      result = Platform::Math.frexp x, exp
      [result, exp.read_int]
    end
  end

  def ldexp(x, n)
    n = Type.coerce_to(n, Integer, :to_int)

    Platform::Math.ldexp Float(x), n
  end

  # Rubinius-specific, used in Marshal
  def modf(x)
    FFI::MemoryPointer.new :double do |integral|
      fractional = Platform::Math.modf x, integral
      [fractional, integral.read_float]
    end
  end

  def hypot(x, y)
    Platform::Math.hypot Float(x), Float(y)
  end

  def erf(x)
    Platform::Math.erf Float(x)
  end

  def erfc(x)
    Platform::Math.erfc Float(x)
  end

  def verify_domain(msg)
    unless yield
      raise Errno::EDOM, msg if Errno.const_defined?(:EDOM)
      raise Errno::ERANGE, msg if Errno.const_defined?(:ERANGE)
    end
  end
end


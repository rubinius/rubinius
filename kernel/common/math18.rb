# -*- encoding: us-ascii -*-

module Math
  def asin(x)
    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'asin' unless x.abs <= 1.0
    FFI::Platform::Math.asin x
  end

  def acos(x)
    x = Rubinius::Type.coerce_to_float(x)

    raise DomainError, 'acos' unless x.abs <= 1.0

    FFI::Platform::POSIX.errno = 0

    ret = FFI::Platform::Math.acos x
    Errno.handle
    ret
  end

  def acosh(x)
    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'acosh' unless x >= 1.0
    FFI::Platform::Math.acosh x
  end

  def log(x)
    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'log' unless x >= 0.0
    FFI::Platform::Math.log x
  end

  def log2(x)
    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'log2' unless x >= 0.0
    FFI::Platform::Math.log2 x
  end

  def log10(x)
    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'log10' unless x >= 0.0
    FFI::Platform::Math.log10 x
  end

  def sqrt(x)
    x = Rubinius::Type.coerce_to_float(x)
    raise DomainError, 'sqrt' unless x >= 0.0
    FFI::Platform::Math.sqrt x
  end

end

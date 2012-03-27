# -*- encoding: us-ascii -*-

module Math
  def log(x)
    x = Rubinius::Type.coerce_to_float_for_numeric(x)
    raise DomainError, 'log' unless x >= 0.0
    FFI::Platform::Math.log x
  end
end

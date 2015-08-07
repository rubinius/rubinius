class Exception
  def self.allocate
    Rubinius.primitive :exception_allocate
    raise PrimitiveFailure, "Exception.allocate primitive failed"
  end

  def capture_backtrace!(offset)
    # replaced in delta with a real implementation
  end
end

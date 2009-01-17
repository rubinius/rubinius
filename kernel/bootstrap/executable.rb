class Executable
  def self.allocate
    Ruby.primitive :executable_allocate
    raise PrimitiveFailure, "Executable.allocate primitive failed"
  end
end

class Executable
  def self.allocate
    Ruby.primitive :executable_allocate
    raise PrimitiveFailure, "Unable to allocate an Executable"
  end
end

class CompiledMethod
  def self.load_from_file(path, version)
    Ruby.primitive :load_file
    raise PrimitiveFailure, "primitive failed"
  end

  def activate_as_script
    Ruby.primitive :activate_as_script
    raise PrimitiveFailure, "primitive failed"
  end

  def compile
    Ruby.primitive :iseq_compile
    raise PrimitiveFailure, "primitive failed"
  end
end

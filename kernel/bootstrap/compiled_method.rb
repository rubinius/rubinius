class CompiledMethod
  def compile
    Ruby.primitive :iseq_compile
    raise PrimitiveFailure, "primitive failed"
  end
end

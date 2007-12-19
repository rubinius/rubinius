class CompiledMethod
  def compile
    Ruby.primitive :iseq_compile
  end
end
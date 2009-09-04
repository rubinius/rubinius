class CompileAsMatcher
  def matches?(actual)
    # TODO: enable AST transforms
    @actual = Rubinius::CompilerNG.compile_test_bytecode actual
    @actual == @expected
  end
end

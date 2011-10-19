class CompileAsMatcher
  def matches?(actual)
    @actual = Rubinius::Compiler.compile_test_bytecode_19 actual, @plugins
    @actual == @expected
  end
end

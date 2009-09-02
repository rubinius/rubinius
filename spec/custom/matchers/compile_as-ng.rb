class CompileAsMatcher
  def matches?(actual)
    compiler = Compiler.new TestGenerator
    # TODO: Fix the compiler to have a proper interface for
    # enabling plugins. All compiler specs should be written
    # without plugins enabled, and each plugin should have
    # specs for bytecode with and without the plugin enabled.
    compiler.instance_variable_get(:@plugins).clear
    @plugins.each { |plugin| compiler.activate plugin }

    node = Rubinius::AST::Snippit.new actual.to_ast

    generator = TestGenerator.new
    node.bytecode generator

    @actual = generator
    @actual == @expected
  end
end

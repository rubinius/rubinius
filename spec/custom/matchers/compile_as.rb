require 'spec/custom/helpers/generator'

# The CompileAsMatcher wraps the logic for checking that a string of Ruby code
# is converted to the expected bytecode. It is combined with the #compile_as
# spec helper and enables specs of the form:
#
#   "a = 1".should compile_as(<some bytecode>)
#
class CompileAsMatcher
  def initialize(expected, plugins)
    @expected = expected
    @plugins = plugins
  end

  def matches?(actual)
    sexp = actual.to_sexp
    generator = TestGenerator.new

    compiler = Compiler.new TestGenerator
    @plugins.each { |plugin| compiler.activate plugin }

    node = compiler.convert_sexp s(:snippit, sexp)
    node.bytecode generator

    @actual = generator
    @actual == @expected
  end

  def failure_message
    ["Expected:\n#{@actual.pretty_inspect}\n",
     "to equal:\n#{@expected.pretty_inspect}"]
  end
end

class Object
  def compile_as(bytecode, *plugins)
    CompileAsMatcher.new bytecode, plugins
  end
end

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
    @actual = Rubinius::Compiler.compile_test_bytecode actual, @plugins
    @actual == @expected
  end

  def diff(actual, expected)
    actual = actual.pretty_inspect.to_a
    expected = expected.pretty_inspect.to_a

    line = actual.each_with_index do |item, index|
      break index unless item == expected[index]
    end

    /^( +)/ =~ actual[line]
    marker = "#{' ' * $1.size if $1}^ differs\n\n"
    actual.insert line+1, marker
    expected.insert line+1, marker

    return actual.join, expected.join
  end

  def failure_message
    actual, expected = diff @actual, @expected
    ["Expected:\n#{actual}\n", "to equal:\n#{expected}"]
  end
end

class Object
  def compile_as(bytecode, *plugins)
    CompileAsMatcher.new bytecode, plugins
  end
end

# Temporary location for modifications needed for any Compiler
# Node classes while getting Melbourne running.

# Use a different matcher while converting the parser/compiler
class CompileAsMatcher
  def matches?(actual)
    node = actual.to_ast
    generator = TestGenerator.new

    compiler = Compiler.new TestGenerator
    # TODO: Fix the compiler to have a proper interface for
    # enabling plugins. All compiler specs should be written
    # without plugins enabled, and each plugin should have
    # specs for bytecode with and without the plugin enabled.
    compiler.instance_variable_get(:@plugins).clear
    @plugins.each { |plugin| compiler.activate plugin }

    node.bytecode generator

    @actual = generator
    @actual == @expected
  end
end

class Compiler
  class Node
    def add(node)
    end

    class Top < Node
      attr_accessor :child

      def add(node)
        @child = node
      end

      def bytecode(g)
        @child.bytecode(g)
      end
    end
  end
end

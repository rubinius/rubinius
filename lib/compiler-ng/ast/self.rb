module Rubinius
  module AST
    class Self < Node
      def self.from(p)
        Self.new p.compiler
      end

      def bytecode(g)
        g.push :self
      end
    end
  end
end

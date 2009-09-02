module Rubinius
  module AST
    class Self < Node
      def bytecode(g)
        g.push :self
      end
    end
  end
end

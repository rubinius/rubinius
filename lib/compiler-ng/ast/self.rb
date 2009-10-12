module Rubinius
  module AST
    class Self < Node
      def bytecode(g)
        g.push :self
      end

      def defined(g)
        g.push_literal "self"
      end

      def constant_defined(s)
        s << "self"
      end
    end
  end
end

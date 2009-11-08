module Rubinius
  module AST
    class Self < Node
      def bytecode(g)
        pos(g)

        g.push :self
      end

      def defined(g)
        g.push_literal "self"
      end

      def constant_defined(s)
        s << "self"
      end

      def receiver_defined(g, f)
        g.push :self
      end
    end
  end
end

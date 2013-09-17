# -*- encoding: us-ascii -*-

module Rubinius
  module AST
    class File < Node
      def bytecode(g)
        pos(g)

        g.push_scope
        g.send :active_path, 0
      end

      def defined(g)
        g.push_literal "expression"
      end

      def to_sexp
        [:file]
      end
    end
  end
end

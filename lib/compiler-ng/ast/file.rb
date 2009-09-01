module Rubinius
  module AST
    class File < Node
      def self.from(p)
        File.new p.compiler
      end

      def bytecode(g)
        pos(g)

        g.push_scope
        g.send :active_path, 0
      end
    end
  end
end

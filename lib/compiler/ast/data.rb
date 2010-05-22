module Rubinius
  module AST
    class EndData < Node
      attr_accessor :data

      def initialize(offset, body)
        @offset = offset
        @body = body
      end

      # When a script includes __END__, Ruby makes the data after it
      # available as an IO instance via the DATA constant. Since code
      # in the toplevel can access this constant, we have to set it up
      # before any other code runs. This AST node wraps the top node
      # returned by the file parser.
      def bytecode(g)
        g.push_rubinius
        g.push_scope
        g.send :data_path, 0
        g.push_literal @offset
        g.send :set_data, 2
        g.pop

        @body.bytecode(g)
      end
    end
  end
end

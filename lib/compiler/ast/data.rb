module Rubinius
  module AST
    class EndData < Node
      attr_accessor :data

      def initialize(line, data)
        @line = line
        @data = data
      end

      # if scope.root_script?
      #   Rubinius.set_data(...)
      # end
      def bytecode(g)
        pos(g)

        not_root = g.new_label
        done = g.new_label

        g.push_scope
        g.send :root_script?, 0
        g.gif not_root
        g.push_const(:Rubinius)
        g.push_literal(data)
        g.send :set_data, 1
        g.goto done
        not_root.set!
        g.push_nil
        done.set!
      end
    end
  end
end

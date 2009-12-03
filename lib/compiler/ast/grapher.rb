module Rubinius
  module AST
    class AsciiGrapher
      def initialize(ast)
        @ast = ast
      end

      def print
        graph_node @ast
      end

      def indented_print(level, value)
        puts "#{" " * level}#{value}"
      end

      def print_node(node, level)
        name = node.class.to_s.split("::").last
        indented_print level, name
      end

      def graph_node(node, level=0)
        print_node node, level
        level += 2

        nodes = []
        node.instance_variables.each do |v|
          next if v == "@compiler"

          value = node.instance_variable_get v

          # lame, yes. remove when Node doesn't have @body by default
          next if v == "@body" and value.nil? and not v.respond_to? :body=

          if value.kind_of? Node
            nodes << [v, value]
          else
            graph_value v, value, level
          end
        end

        nodes.each do |name, node|
          puts "#{" " * level}#{name}: \\"
          graph_node node, level
        end
      end

      def graph_simple(name, value, level)
        puts "#{" " * level}#{name}: #{value}"
      end

      def graph_value(name, value, level)
        case value
        when NilClass, String
          graph_simple name, value.inspect, level
        when TrueClass, FalseClass, Symbol, Fixnum
          graph_simple name, value, level
        when Array
          puts "#{" " * level}#{name}: \\"
          nodes = []
          value.each do |v|
            if v.kind_of? Node
              nodes << v
            else
              graph_value "-", v, level + 2
            end
          end

          nodes.each { |n| graph_node n, level + 2 }
        else
          graph_simple name, value.class, level
        end
      end
    end
  end
end

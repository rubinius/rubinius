# -*- encoding: us-ascii -*-

module Rubinius
  module AST
    class AsciiGrapher
      def initialize(ast, node_kind=Node)
        @ast = ast
        @node_kind = node_kind
      end

      def print
        graph_node @ast
      end

      def indented_print(level, value)
        puts "#{" " * level}#{value}"
      end

      def print_node(node, level, idx=nil)
        name = node.class.to_s.split("::").last

        name = "#{name} [#{idx}]" if idx

        indented_print level, name
      end

      def graph_node(node, level=0, idx=nil)
        print_node node, level, idx
        level += 2

        nodes = []
        node.instance_variables.each do |v|
          next if v == "@compiler"

          value = node.instance_variable_get v

          # lame, yes. remove when Node doesn't have @body by default
          next if v == "@body" and value.nil? and not v.respond_to? :body=

          if value.kind_of? @node_kind
            nodes << [v, value]
          else
            graph_value v, value, level
          end
        end

        nodes.each do |name, n|
          puts "#{" " * level}#{name}: \\"
          graph_node n, level + 2
        end
      end

      def graph_simple(name, value, level)
        puts "#{" " * level}#{name}: #{value}"
      end

      def graph_value(name, value, level)
        case value
        when NilClass, String
          graph_simple name, value.inspect, level
        when Symbol
          puts "#{" " * level}#{name}: :#{value}"
        when TrueClass, FalseClass, Fixnum
          graph_simple name, value, level
        when Array
          if value.empty?
            puts "#{" " * level}#{name}: []"
          else
            puts "#{" " * level}#{name}: ["
            nodes = []
            value.each_with_index do |v,i|
              if v.kind_of? @node_kind
                nodes << [v, i]
              else
                graph_value "[#{i}] ", v, level + 2
              end
            end

            nodes.each { |n| graph_node n[0], level + 2, n[1] }

            puts "#{' ' * level}]"
          end
        else
          graph_simple name, value.class, level
        end
      end
    end
  end
end

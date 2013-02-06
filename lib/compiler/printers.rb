# -*- encoding: us-ascii -*-

module Rubinius
  class Compiler
    class Printer < Stage
      def initialize
      end
    end

    class ASTPrinter < Printer
      def run
        @input.ascii_graph
        @output = @input
        run_next
      end
    end

    class SexpPrinter < Printer
      def run
        require 'pp'

        puts @input.to_sexp.pretty_inspect
        @output = @input
        run_next
      end
    end

    class MethodPrinter < Printer
      attr_accessor :bytecode, :assembly

      SEPARATOR_SIZE = 40

      def method_names=(names)
        return if names.empty?
        @method_names = names.map { |n| n.to_sym }
      end

      def match?(name)
        return true unless @method_names
        @method_names.include? name
      end

      def print_header(code)
        name = code.name.inspect
        size = (SEPARATOR_SIZE - name.size - 2) / 2
        size = 1 if size <= 0
        puts "\n#{"=" * size} #{name} #{"=" * (size + name.size % 2)}"
        print "Arguments:   "
        print "#{code.required_args} required, "
        print "#{code.post_args} post, "
        print "#{code.total_args} total"
        print code.splat ? ", (splat #{code.splat})\n" : "\n"
        puts "Arity:       #{code.arity}"
        print "Locals:      #{code.local_count}"
        print code.local_count > 0 ? ": #{code.local_names.join ", "}\n" : "\n"
        puts "Stack size:  #{code.stack_size}"
        literals = code.literals.collect do |literal|
          case literal
          when CompiledCode
            "<compiled code>"
          else
            literal.inspect
          end
        end
        print "Literals:    #{literals.size}"
        print code.literals.size > 0 ? ": #{literals.join ", "}\n" : "\n"
        print_lines code
        puts
      end

      def print_footer
        puts "-" * SEPARATOR_SIZE
      end

      def print_lines(code)
        lines = code.lines
        size = lines.size - 1
        i = 1

        if lines[0] == -1
          puts  "Line:        #{lines[1]}"
          i += 2
        end

        print "Lines to IP: "
        while i < size
          print "#{lines[i]}: #{lines[i - 1]}..#{lines[i + 1] - 1}"
          i += 2
          print ", " if i < size
        end
        puts
      end

      def print_method(code)
        if match? code.name
          print_header code
          puts code.decode if @bytecode
          print_footer
        end

        code.literals.each do |m|
          next unless m.kind_of? Rubinius::CompiledCode
          print_method m
        end
      end

      def run
        print_method @input

        @output = @input
        run_next
      end
    end
  end
end

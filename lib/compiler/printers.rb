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

      def print_header(cm)
        name = cm.name.inspect
        size = (SEPARATOR_SIZE - name.size - 2) / 2
        size = 1 if size <= 0
        puts "\n#{"=" * size} #{name} #{"=" * (size + name.size % 2)}"
        print "Arguments:   "
        print "#{cm.required_args} required, #{cm.total_args} total"
        print cm.splat ? ", (splat)\n" : "\n"
        print "Locals:      #{cm.local_count}"
        print cm.local_count > 0 ? ": #{cm.local_names.join ", "}\n" : "\n"
        puts "Stack size:  #{cm.stack_size}"
        print_lines cm
        puts
      end

      def print_footer
        puts "-" * SEPARATOR_SIZE
      end

      def print_lines(cm)
        print "Lines to IP: "
        size = cm.lines.size - 1
        i = 1
        while i < size
          print "#{cm.lines[i]}: #{cm.lines[i - 1]}-#{cm.lines[i + 1] - 1}"
          i += 2
          print ", " if i < size
        end
        puts
      end

      def print_decoded(cm)
        return unless match? cm.name

        print_header cm
        puts cm.decode if @bytecode

        if @assembly
          puts
          mm = cm.make_machine_method
          mm.disassemble
        end
        print_footer
      end

      def print_method(cm)
        print_decoded cm

        cm.literals.each do |m|
          next unless m.kind_of? Rubinius::CompiledMethod
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

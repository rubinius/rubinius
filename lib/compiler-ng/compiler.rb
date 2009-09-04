module Rubinius
  class CompilerNG
    attr_accessor :parser, :generator, :encoder, :writer

    # Temporary
    def self.enable
      Compiler.module_eval do
        class << self
          alias_method :old_compile_file, :compile_file
          alias_method :old_compile_string, :compile_string

          define_method :compile_file, CompilerNG.method(:compile_file)
          define_method :compile_string, CompilerNG.method(:compile_eval)
        end
      end
    end

    def self.compile(file, output=nil)
    end

    def self.compile_file(file, line=1)
      compiler = new :file, :compiled_file

      parser = compiler.parser
      parser.root AST::Script
      parser.input file, line

      compiler.run
    end

    def self.compile_string(string, file="(eval)", line=1)
    end

    def self.compile_eval(string, binding, file="(eval", line=1)
    end

    def self.compile_test_bytecode(string, name="(eval)")
      compiler = new :string, :bytecode

      parser = compiler.parser
      parser.root AST::Snippit
      parser.input string, name
      # TODO: handle AST transforms

      compiler.generator.processor TestGenerator

      compiler.run
    end

    def initialize(from, to)
      @start = Stages[from].new self, to
    end

    def run
      @start.run
    end
  end
end

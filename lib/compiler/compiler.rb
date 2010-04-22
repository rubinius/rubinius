module Rubinius

  class CompileError < RuntimeError
  end

  class Compiler
    attr_accessor :parser, :generator, :encoder, :packager, :writer

    def self.compiler_error(msg, orig)
      if defined?(RUBY_ENGINE) and RUBY_ENGINE == "rbx"
        raise Rubinius::CompileError, msg, orig
      else
        orig.message.replace("#{orig.message} - #{msg}")
        raise orig
      end
    end

    def self.compiled_name(file)
      if file.suffix? ".rb"
        file + "c"
      else
        file + ".compiled.rbc"
      end
    end

    def self.compile(file, output=nil, line=1, transforms=:default)
      compiler = new :file, :compiled_file

      parser = compiler.parser
      parser.root AST::Script

      if transforms.kind_of? Array
        transforms.each { |t| parser.enable_category t }
      else
        parser.enable_category transforms
      end

      parser.input file, line

      writer = compiler.writer
      writer.name = output ? output : compiled_name(file)

      begin
        compiler.run
      rescue Exception => e
        compiler_error "Error trying to compile #{file}", e
      end

    end

    # Match old compiler's signature
    def self.compile_file_old(file, flags=nil)
      compile_file file, 1
    end

    def self.compile_file(file, line=1)
      compiler = new :file, :compiled_method

      parser = compiler.parser
      parser.root AST::Script
      parser.default_transforms
      parser.input file, line

      begin
        compiler.run
      rescue Exception => e
        compiler_error "Error trying to compile #{file}", e
      end
    end

    def self.compile_string(string, file="(eval)", line=1)
      compiler = new :string, :compiled_method

      parser = compiler.parser
      parser.root AST::Script
      parser.default_transforms
      parser.input string, file, line

      compiler.run
    end

    def self.compile_eval(string, variable_scope, file="(eval)", line=1)
      compiler = new :string, :compiled_method

      parser = compiler.parser
      parser.root AST::EvalExpression
      parser.default_transforms
      parser.input string, file, line

      compiler.generator.variable_scope = variable_scope

      compiler.run
    end

    def self.compile_test_bytecode(string, transforms)
      compiler = new :string, :bytecode

      parser = compiler.parser
      parser.root AST::Snippet
      parser.input string
      transforms.each { |x| parser.enable_transform x }

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

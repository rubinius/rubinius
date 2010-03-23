module Rubinius
  class Compiler
    Stages = { }

    class Stage
      attr_accessor :next_stage, :printer

      def self.stage(name)
        @stage = name
        Stages[name] = self
      end

      def self.stage_name
        @stage
      end

      def self.next_stage(klass)
        @next_stage_class = klass
      end

      def self.next_stage_class
        @next_stage_class
      end

      def initialize(compiler, last)
        @next_stage = create_next_stage compiler, last
      end

      def input(data)
        @input = data
      end

      def processor(klass)
        @processor = klass
      end

      def create_next_stage(compiler, last)
        return if self.class.stage_name == last

        stage = self.class.next_stage_class
        stage.new compiler, last if stage
      end

      def insert(stage)
        @next_stage, stage.next_stage = stage.next_stage, self
      end

      def run_next
        if @next_stage
          @next_stage.input @output
          @next_stage.run
        else
          @output
        end
      end
    end

    # compiled method -> compiled file
    class Writer < Stage
      stage :compiled_file

      attr_accessor :name

      def initialize(compiler, last)
        super
        compiler.writer = self
        @processor = Rubinius::CompiledFile
      end

      def run
        @name = "#{@input.file}c" unless @name
        @processor.dump @input, @name
        @input
      end
    end

    # encoded bytecode -> compiled method
    class Packager < Stage
      stage :compiled_method
      next_stage Writer

      def initialize(compiler, last)
        super
        compiler.packager = self
      end

      def print(klass=MethodPrinter)
        @printer = klass.new
        @printer.insert self
        @printer
      end

      def run
        @output = @input.package Rubinius::CompiledMethod
        run_next
      end
    end

    # symbolic bytecode -> encoded bytecode
    class Encoder < Stage
      stage :encoded_bytecode
      next_stage Packager

      def initialize(compiler, last)
        super
        compiler.encoder = self
        @encoder = InstructionSequence::Encoder
      end

      def processor(encoder)
        @encoder = encoder
      end

      def run
        @input.encode @encoder
        @output = @input
        run_next
      end
    end

    # AST -> symbolic bytecode
    class Generator < Stage
      stage :bytecode
      next_stage Encoder

      attr_accessor :variable_scope

      def initialize(compiler, last)
        super
        @variable_scope = nil
        compiler.generator = self
        @processor = Rubinius::Generator
      end

      def run
        @output = @processor.new
        @input.variable_scope = @variable_scope
        @input.bytecode @output
        @output.close
        run_next
      end
    end

    # source -> AST
    class Parser < Stage
      attr_accessor :transforms

      def initialize(compiler, last)
        super
        compiler.parser = self
        @transforms = []
        @processor = Rubinius::Melbourne
      end

      def root(klass)
        @root = klass
      end

      def default_transforms
        @transforms.concat AST::Transforms.category(:default)
      end

      def print(klass=ASTPrinter)
        @printer = klass.new
        @printer.insert self
        @printer
      end

      def enable_category(name)
        transforms = AST::Transforms.category name
        @transforms.concat transforms if transforms
      end

      def enable_transform(name)
        transform = AST::Transforms[name]
        @transforms << transform if transform
      end

      def create
        @parser = @processor.new(@file, @line, @transforms)
        @parser.magic_handler = self
        @parser
      end

      def add_magic_comment(str)
        if m = /-\*-\s*(.*?)\s*(-\*-)$/.match(str)
          enable_transform(m[1].to_sym)
        end
      end

      def run
        @output = @root.new parse
        @output.file = @file
        run_next
      end
    end

    # source file -> AST
    class FileParser < Parser
      stage :file
      next_stage Generator

      def input(file, line=1)
        @file = file
        @line = line
      end

      def parse
        create.parse_file
      end
    end

    # source string -> AST
    class StringParser < Parser
      stage :string
      next_stage Generator

      def input(string, name="(eval)", line=1)
        @input = string
        @file = name
        @line = line
      end

      def parse
        create.parse_string(@input)
      end
    end
  end
end

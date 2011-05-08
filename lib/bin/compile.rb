require File.expand_path("../../compiler", __FILE__)

class CompilerScript
  def initialize
    @print_ast    = nil
    @pattern      = nil
    @output_name  = nil
    @transforms   = []
    @method_names = []
    @files        = []
    @strings      = []
    @stdin        = false
  end

  def options(argv=ARGV)
    @options = Rubinius::Options.new "Usage: rbx compile [options] [files] [dirs]", 28

    @options.doc " How to specify the input"

    @options.on "-", "Read from STDIN" do
      @stdin = true
    end

    @options.on "-e", "STRING", "Compile STRING" do |s|
      @strings << s
    end


    @options.doc "\n How to specify the output file"

    @options.on "-o", "--output", "NAME", "Compile single input file to NAME" do |n|
      @output_name = n
    end

    @options.on("-s", "--replace", "FORM",
               "Transform filename, where FORM is pattern:replacement") do |s|
      pattern, @replacement = s.split(":")
      @pattern = Regexp.new pattern
    end


    @options.doc "\n How to transform the AST"
    @options.doc "\n  The default category of transforms are enabled unless specific"
    @options.doc "  ones are selected or --no-transform is given"
    @options.doc ""

    @options.on "-t", "--transform", "NAME", "Enable AST transform NAME" do |t|
      transform = Rubinius::AST::Transforms[t.to_sym]
      @transforms << transform if transform
    end

    @options.on("-T", "--transforms", "NAME",
                "Enable NAME category of AST transforms") do |c|
      transforms = Rubinius::AST::Transforms.category c.to_sym
      @transforms.concat transforms if transforms
    end

    @options.on "--no-transform", "Do not transform the AST" do
      @no_transforms = true
    end

    @options.doc "\n     where the transforms are:"
    @options.doc "       Category: all"
    @options.doc "         Includes all transforms\n"

    Rubinius::AST::Transforms.category_map.each do |category, transforms|
      @options.doc "       Category: #{category}"
      transforms.each do |t|
        text = "         %-19s  %s" % [t.transform_name, t.transform_comment]
        @options.doc text
      end
      @options.doc ""
    end


    @options.doc " How to print representations of data structures"

    @options.on "-A", "--print-ast", "Print an ascii graph of the AST" do
      @print_ast = Rubinius::Compiler::ASTPrinter
    end

    @options.on "-S", "--print-sexp", "Print the AST as an S-expression" do
      @print_ast = Rubinius::Compiler::SexpPrinter
    end

    @options.on "-B", "--print-bytecode", "Print bytecode for compiled methods" do
      @print_bytecode = true
    end

    @options.on "-N", "--method", "NAME", "Only decode methods named NAME" do |n|
      @method_names << n
    end

    @options.on "-P", "--print", "Enable all stage printers" do
      @print_ast = Rubinius::Compiler::ASTPrinter
      @print_bytecode = true
    end


    @options.doc "\n How to modify runtime behavior"

    @options.on "-i", "--ignore", "Continue on errors" do
      @ignore = true
    end


    @options.doc "\n Help!"

    @options.on "-V", "--verbose", "Print processing information" do
      @verbose = true
    end

    @options.help
    @options.doc ""

    @sources = @options.parse argv
  end

  def enable_transforms(parser)
    return if @no_transforms
    if @transforms.empty?
      parser.default_transforms
    else
      parser.transforms = @transforms
    end
  end

  def set_printers(compiler)
    compiler.parser.print @print_ast if @print_ast

    if @print_bytecode
      packager = compiler.packager
      printer = packager.print
      printer.bytecode = @print_bytecode
      printer.method_names = @method_names
    end
  end

  def set_output(compiler, name)
    if writer = compiler.writer
      writer.name = name
    end
  end

  def protect(name)
    begin
      yield
    rescue Object => e
      puts "Failed compiling #{name}"
      if @ignore
        puts e.awesome_backtrace
      else
        raise e
      end
    end
  end

  def collect_files
    @sources.each do |entry|
      if File.directory? entry
        spec = "#{entry}/**/*.rb"
      else
        spec = entry
      end

      @files += Dir[spec]
    end
  end

  def compile_files
    collect_files

    @files.each do |file|
      puts file if @verbose

      if @pattern
        output = file.gsub(@pattern, @replacement)
        output << "c"
      else
        output = @output_name
      end

      protect file do
        compiler = Rubinius::Compiler.new :file, :compiled_file

        parser = compiler.parser
        parser.root Rubinius::AST::Script
        parser.input file
        enable_transforms parser

        set_printers compiler
        set_output compiler, output

        compiler.run
      end
    end
  end

  def compile_string(string, origin)
    if @output_name
      compiler = Rubinius::Compiler.new :string, :compiled_file
      set_output compiler, @output_name
    else
      compiler = Rubinius::Compiler.new :string, :compiled_method
    end

    parser = compiler.parser
    parser.root Rubinius::AST::Script
    enable_transforms parser
    parser.input string, origin, 1

    set_printers compiler

    compiler.run
  end

  def compile_strings
    @strings.each do |string|
      compile_string string, "(snippet)"
    end
  end

  def compile_stdin
    compile_string $stdin.read, "(stdin)" if @stdin
  end

  def main
    options
    compile_stdin
    compile_strings
    compile_files
  end
end

CompilerScript.new.main

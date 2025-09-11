# This is a temporary script to compile a new CodeDB cache from an existing
# bootstrap cache. It's pretty messy. Hopefully, it will be short-lived.

require "rubinius/code/toolset"

Rubinius::ToolSets.create :build do
  require "rubinius/code/melbourne"
  require "rubinius/code/processor"
  require "rubinius/code/compiler"
  require "rubinius/code/ast"
end

require File.expand_path("../config/config.rb", __dir__)

class CodeDBCompiler
  def self.compile(file, line=1, transforms=[:default, :kernel, :experimental])
    compiler = Rubinius::ToolSets::Build::Compiler.new :file, :compiled_code

    parser = compiler.parser
    parser.root Rubinius::ToolSets::Build::AST::Script

    if transforms.kind_of? Array
      transforms.each { |t| parser.enable_category t }
    else
      parser.enable_category transforms
    end

    parser.input file, line

    generator = compiler.generator
    generator.processor Rubinius::ToolSets::Build::Generator

    compiler.run
  end

  def self.marshal(code)
    marshaler = Rubinius::ToolSets::Build::CompiledFile::Marshal.new
    marshaler.marshal code
  end
end

class CodeDBWriter
  MAGIC = "!RBIX\n"
  VERSION = "   1\n"
  SIGNATURE_HASH = 0
  SIZE_FORMAT = "%15d\n"

  def initialize(path)
    @path = path
    @init_files = []
    @source_files = []
    @init = ""
    @index = ""
    @contents = ""
    @data = ""
  end

  def add_init(source)
    @init_files << source
  end

  def add_source(source)
    @source_files << source
  end

  def add_code(name, id, code)
    @contents << "#{name.size} #{name} 0 0 #{id} 0\n"

    cache = [[id, code]]

    while x = cache.shift
      id, cc = x

      cc.literals.each_with_index do |value, index|
        if value.kind_of? Rubinius::CompiledCode
          cc.literals[index] = i = value.code_id
          cache.unshift [i, value]
        end
      end

      marshaled = CodeDBCompiler.marshal cc

      @index << "#{id} #{@data.size} #{marshaled.size}\n"

      @data << marshaled
    end
  end

  def compile_file(file)
    puts "RBC #{file}"

    code = CodeDBCompiler.compile file

    return file, code.code_id, code
  end

  def compile
    @init_files.each do |file|
      name, id, code = compile_file file

      add_code name, id, code

      @init << "#{id}\n"
    end

    @source_files.each do |file|
      add_code(*compile_file(file))
    end
  end

  def write
    File.open @path, "wb" do |f|
      f.write MAGIC
      f.write VERSION
      f.write "%64d\n" % SIGNATURE_HASH
      f.write SIZE_FORMAT % @init.size
      f.write SIZE_FORMAT % @data.size
      f.write SIZE_FORMAT % @data.size
      f.write SIZE_FORMAT % @index.size
      f.write SIZE_FORMAT % @contents.size
      f.write @init
      f.write @data
      f.write @index
      f.write @contents
    end
  end
end

path = ARGV.shift
puts "Building CodeDB cache: #{path}"

db_writer = CodeDBWriter.new path

Dir.chdir "#{Rubinius::BUILD_CONFIG[:builddir]}#{Rubinius::BUILD_CONFIG[:codedbdir]}/source" do
  File.foreach "#{Rubinius::BUILD_CONFIG[:sourcedir]}/core/load_order.txt" do |name|
    db_writer.add_init "core/#{name.chomp}"
  end

  Dir["**/*.rb"].each do |name|
    next if name.start_with? "core/"

    db_writer.add_source name
  end

  db_writer.compile
end

db_writer.write

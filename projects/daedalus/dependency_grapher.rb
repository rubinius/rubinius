# A simple processor to create a list of dependencies for a given source code
# file based on the #include files in the source file.
#
# The process involves three phases:
#
#   1. Parsing the source file for all C/C++ preprocessor control directives
#      (#include, #ifdef, #ifndef, #else, #endif, #define, #undef, #if, #elif)
#   2. Executing the tree derived from step 1 in the context of a set of
#      macro definitions.
#   3. Collecting the dependency list for each source file.
#
# In step 1, the tree resulting from parsing each included file is cached. If
# a later file of the same expanded relative path is included, the parse tree
# from the cache is returned.
#
# The system level macro definitions are retrieved using `cpp -dM`. This
# system set may be updated with new definitions or removed definitions while
# the parse tree is executed.

require 'set'

module Daedalus
class DependencyGrapher
  class ExpressionEvaluator
    def initialize(expression)
      @expression = expression
    end

    # TODO: when melbourne is running in MRI, use it to parse
    # the expression and then evaluate it.
    def evaluate(defines)
      @expression == "0" ? false : true
    end
  end

  class Node
    def initialize(parser)
      @parser = parser
    end

    def close
      message = "unbalanced \#endif for #{@parser.stack_top.class} at line #{@parser.line}"
      raise ParseError, message
    end

    def add_else
      message = "invalid \#else for #{@parser.stack_top.class} at line #{@parser.line}"
    end

    # TODO: remove
    def execute(defines, node)
      puts "#execute not implemented for #{self.class}"
    end
  end

  module Container
    attr_accessor :body

    def initialize(parser)
      super parser
      @body = []
    end

    def close
      @parser.stack_pop
    end

    def execute_body(defines, node)
      body.each { |x| x.execute(defines, node) }
    end
  end

  module Conditional
    def initialize(parser)
      super parser
      @else = nil
    end

    def add_else(node)
      @else = node
    end
  end


  class SourceFile < Node
    include Container

    attr_reader :name, :object_name, :includes, :dependencies

    def initialize(name, parser)
      super parser
      @name = name
      @object_name = name.sub(/((c(pp)?)|S)$/, 'o')
      @includes = []
    end

    def execute(defines)
      execute_body defines, self
    end

    def collect_dependencies
      set = Set.new

      set << @name
      @includes.each { |x| x.collect_dependencies(set) }

      @dependencies = set.to_a.sort
    end

    def print_dependencies(out, max)
      str = "#{@object_name}:"
      out.print str

      width = str.size
      @dependencies.each do |name|
        size = name.size + 1
        if width + size > max
          width = 0
          out.print " \\\n "
        end

        out.print " ", name
        width += size
      end

      out.print "\n"
    end
  end

  class IncludedFile < Node
    include Container

    attr_reader :name, :includes

    def self.cache
      @cache ||= {}
    end

    def initialize(name, parser)
      super parser
      @name = name
      @includes = []
    end

    def expand_filename(node)
      return if File.exist? @name

      @parser.directories.each do |dir|
        path = File.join dir, @name
        return @name = path if File.file? path
      end

      # Try to find the file in the same directory as where we're looking from
      dir = File.dirname(node.name)
      path = File.join dir, @name

      return @name = path if File.file?(path)

      raise Errno::ENOENT, "unable to find file to include: #{@name} from #{node.name}"
    end

    def execute(defines, node)
      expand_filename(node)

      if cached = self.class.cache[@name.to_sym]
        @body = cached.body
      else
        parser = FileParser.new self, @parser.directories
        parser.parse_file @name
        self.class.cache[@name.to_sym] = self
      end

      execute_body defines, self

      node.includes << self
    end

    def collect_dependencies(set)
      return if set.include? @name

      set << @name
      @includes.each { |x| x.collect_dependencies(set) }
    end
  end

  class IfDefined < Node
    include Conditional, Container

    def initialize(macro, parser)
      super parser
      @macro = macro.strip
    end

    def execute(defines, node)
      if defines.key? @macro
        execute_body defines, node
      elsif @else
        @else.execute(defines, node)
      end
    end
  end

  class IfNotDefined < Node
    include Conditional, Container

    def initialize(macro, parser)
      super parser
      @macro = macro.strip
    end

    def execute(defines, node)
      if !defines.key? @macro
        execute_body defines, node
      elsif @else
        @else.execute(defines, node)
      end
    end
  end

  class If < Node
    include Conditional, Container

    def initialize(expression, parser)
      super parser
      @value = nil
      @expression = expression.strip
    end

    def execute(defines, node)
      @value = ExpressionEvaluator.new(@expression).evaluate defines

      if @value
        execute_body(defines, node)
      elsif @else
        @else.execute(defines, node)
      end
    end
  end

  class Else < Node
    include Container

    def execute(defines, node)
      execute_body defines, node
    end
  end

  class ElseIf < If; end

  class Define < Node
    def initialize(macro, parser)
      super parser

      macro.strip!
      if index = macro.index(" ")
        @name = macro[0..index-1]
        @value = macro[index+1..-1]
        @name, @value = macro.strip.split
      else
        @name = macro
        @value = "1"
      end
    end

    def execute(defines, node)
      defines[@name] = @value
    end
  end

  class Undefine < Node
    def initialize(macro, parser)
      super parser
      @macro = macro.strip
    end

    def execute(defines, node)
      defines.delete @macro
    end
  end

  class ParseError < Exception; end

  # Parses a file for all preprocessor control directives into a tree of Node
  # objects. The parser can operate on a file or an array of lines.
  class FileParser
    attr_reader :line, :directories

    def initialize(root, directories)
      @stack = [root]
      @directories = directories
    end

    # Parser operations

    def add_body(node)
      @stack.last.body << node
    end

    def add_else(node)
      @stack.last.add_else node
    end

    def stack_push(node)
      @stack.push node
    end

    def stack_pop
      @stack.pop
    end

    def stack_top
      @stack.last
    end

    def close
      @stack.last.close
    end

    # Events

    def process_include(name)
      # We do not process any <files>. This could be enabled as
      # an option, but we don't need it or want it now.
      name =~ /\s*"([^"]+)".*$/
      return unless $1

      node = IncludedFile.new $1, self
      add_body node
    end

    def process_ifdef(macro)
      node = IfDefined.new macro, self
      add_body node
      stack_push node
    end

    def process_ifndef(macro)
      node = IfNotDefined.new macro, self
      add_body node
      stack_push node
    end

    def process_endif(rest)
      close
    end

    def process_else(rest)
      node = Else.new self
      add_else node
    end

    def process_define(macro)
      node = Define.new macro, self
      add_body node
    end

    def process_undef(macro)
      node = Undefine.new macro, self
      add_body node
    end

    def process_if(expression)
      node = If.new expression, self
      add_body node
      stack_push node
    end

    def process_elif(expression)
      node = ElseIf.new expression, self
      add_else node
      add_body node
      stack_push node
    end

    # Parse methods

    def parse_file(name)
      parse IO.readlines(name)
    end

    def parse(lines)
      @line = 0

      lines.each do |line|
        @line += 1
        m = line.match(/^\s*#(include|ifdef|ifndef|endif|else|define|undef|if|elif)(.*)$/)

        # TODO: continue reading if line ends in \

        send :"process_#{m[1]}", m[2] if m
      end
    end
  end

  attr_accessor :file_names, :directories, :defines, :system_defines
  attr_reader :sources

  def initialize(files, directories=[], defines=nil)
    @file_names = files
    @directories = directories
    @defines = defines

    @system_defines = {}
    @sources = []
  end

  def get_system_defines
    lines = `cpp -dM #{@defines} /dev/null`.split("\n")

    source = SourceFile.new "sytem_defines", self
    parser = FileParser.new source, @directories
    parser.parse lines

    source.execute @system_defines
  end

  def process
    get_system_defines

    @file_names.each do |name|
      source = SourceFile.new name, self
      parser = FileParser.new source, @directories

      parser.parse_file name
      source.execute @system_defines.dup
      source.collect_dependencies

      @sources << source
    end
  end

  def print_dependencies(out, max=72)
    @sources.each do |source|
      source.print_dependencies out, max
    end
  end
end
end


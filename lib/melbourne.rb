require Rubinius::PARSER_EXT_PATH
require Rubinius::PARSER_PATH + "/processor"

class String
  def to_ast(name="(eval)", line=1)
    Rubinius::Melbourne.parse_string self, name, line
  end

  def to_sexp(name="(eval)", line=1)
    to_ast(name, line).to_sexp
  end
end

class File
  def self.to_ast(name, line=1)
    Rubinius::Melbourne.parse_file name, line
  end

  def self.to_sexp(name, line=1)
    to_ast(name, line).to_sexp
  end
end

module Rubinius
  class Melbourne
    attr_accessor :transforms
    attr_accessor :magic_handler
    attr_accessor :references
    attr_reader   :pre_exe

    def self.parse_string(string, name="(eval)", line=1)
      system_parser.new(name, line).parse_string string
    end

    def self.parse_file(name, line=1)
      system_parser.new(name, line).parse_file
    end

    def self.system_parser
      case
      when Rubinius.ruby18?
        Melbourne
      when Rubinius.ruby19?
        Melbourne19
      when Rubinius.ruby20?
        Melbourne20
      else
        raise Exception, "no processor is defined for Parser compiler stage."
      end
    end

    def initialize(name, line, transforms=[])
      @name = name
      @line = line
      @transforms = transforms
      @magic_handler = nil
      @data_offset = nil
      @pre_exe = []

      # There can be multiple reported, we need to track them all.
      @syntax_errors = []
    end

    attr_reader :syntax_errors

    def add_pre_exe(node)
      @pre_exe << node if node
    end

    def add_magic_comment(str)
      if @magic_handler
        @magic_handler.add_magic_comment str
      end
    end

    def process_data(offset)
      @data_offset = offset
    end

    def syntax_error
      raise @syntax_errors[0] unless @syntax_errors.empty?
    end

    alias_method :file_to_ast, :file_to_ast_18
    alias_method :string_to_ast, :string_to_ast_18

    def parse_string(string)
      syntax_error unless ast = string_to_ast(string, @name, @line)
      ast
    end

    def parse_file
      unless @name and File.exists? @name
        raise Errno::ENOENT, @name.inspect
      end

      syntax_error unless ast = file_to_ast(@name, @line)
      ast = AST::EndData.new @data_offset, ast if @data_offset
      ast
    end

    def process_transforms(line, receiver, name, arguments, privately=false)
      @transforms.each do |transform|
        next unless transform.transform_kind == :call

        if node = transform.match?(line, receiver, name, arguments, privately)
          unless node.kind_of? AST::Node
            node = transform.new line, receiver, name, arguments, privately
          end
          return node
        end
      end
      nil
    end
  end

  class Melbourne19 < Melbourne
    alias_method :file_to_ast, :file_to_ast_19
    alias_method :string_to_ast, :string_to_ast_19
  end

  class Melbourne20 < Melbourne19
  end
end

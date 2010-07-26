base = File.expand_path "../", __FILE__

unless Object.const_defined?(:RUBY_ENGINE) && RUBY_ENGINE == "rbx"
  ext_dir = "ruby"
  require base + "/mri_bridge"
else
  ext_dir = Object.const_get :RUBY_ENGINE
end

require base + "/ext/melbourne/#{ext_dir}/melbourne"
require base + "/melbourne/processor"

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

    def self.parse_string(string, name="(eval)", line=1)
      new(name, line).parse_string string
    end

    def self.parse_file(name, line=1)
      new(name, line).parse_file
    end

    def initialize(name, line, transforms=[])
      @name = name
      @line = line
      @transforms = transforms
      @magic_handler = nil
      @data_offset = nil

      # There can be multiple reported, we need to track them all.
      @syntax_errors = []
    end

    attr_reader :syntax_errors

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
end

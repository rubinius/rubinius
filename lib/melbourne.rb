unless Object.const_defined?(:RUBY_ENGINE) && RUBY_ENGINE == "rbx"
  ext_dir = "ruby"
  require "mri_bridge"
else
  ext_dir = Object.const_get :RUBY_ENGINE
end

require "ext/melbourne/#{ext_dir}/melbourne"
require "melbourne/processor"

class String
  def to_ast(name="(eval)", line=1)
    Rubinius::Melbourne.parse_string self, name, line
  end
end

class File
  def self.to_ast(name, line=1)
    Rubinius::Melbourne.parse_file name, line
  end
end

module Rubinius
  class Melbourne
    attr_accessor :transforms

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
    end

    def syntax_error
      raise @exc if @exc
    end

    def parse_string(string)
      syntax_error unless ast = string_to_ast(string, @name, @line)
      ast
    end

    def parse_file
      syntax_error unless ast = file_to_ast(@name, @line)
      ast
    end

    def process_transforms(line, receiver, name, arguments, privately=false)
      @transforms.each do |transform|
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

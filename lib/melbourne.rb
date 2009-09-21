require "ext/melbourne/melbourne"
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

    def process_transforms(line, receiver, name, arguments)
      @transforms.each do |transform|
        if transform.match? receiver, name, arguments
          return transform.new line, receiver, name, arguments
        end
      end
      nil
    end

  end
end

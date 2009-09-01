require "ext/melbourne/melbourne"
require "melbourne/processor"

class String
  def to_ast(name="(eval)", line=1)
    processor = Rubinius::Melbourne.new name
    unless ast = processor.string_to_ast(self, name, line)
      processor.syntax_error
    end

    ast
  end
end

class File
  def self.to_ast(name, start=1)
    processor = Rubinius::Melbourne.new name
    unless ast = processor.file_to_ast(name, start)
      processor.syntax_error
    end

    ast
  end
end

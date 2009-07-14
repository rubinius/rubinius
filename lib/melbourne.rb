require "ext/melbourne/melbourne"
require "melbourne/processor"
require "melbourne/nodes"

class String
  def to_ast(name="(eval)", line=1)
    processor = Rubinius::Melbourne.new
    unless error = processor.string_to_ast(self, name, line)
      return processor.top
    end

    exc = SyntaxError.new error[0]
    exc.import_position error[1], error[2], error[3]
    exc.file = name
    raise exc
  end
end

class File
  def self.to_ast(name, start=1)
    processor = Rubinius::Melbourne.new
    unless error = processor.file_to_ast(name, start)
      return processor.top
    end

    exc = SyntaxError.new error[0]
    exc.import_position error[1], error[2], error[3]
    exc.file = name
    raise exc
  end
end

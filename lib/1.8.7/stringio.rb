require File.dirname(__FILE__) + '/../stringio'

class StringIO
  def each_byte
    return to_enum :each_byte unless block_given?
    raise IOError, "not opened for reading" unless @readable
    if @pos < @string.length
      @string[@pos..-1].each_byte { |b| @pos += 1; yield b}
    end
    nil
  end

  def each(sep = $/)
    return to_enum :each_byte, sep unless block_given?
    raise IOError, "not opened for reading" unless @readable
    sep = StringValue(sep) unless sep.nil?
    while line = self.getline(sep)
      yield line
    end
    self
  end
  alias_method :each_line, :each
end
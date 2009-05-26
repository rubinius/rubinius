require File.dirname(__FILE__) + '/../stringio'

class StringIO
  def chars
    to_enum :each_char
  end

  def each_byte
    return to_enum :each_byte unless block_given?
    raise IOError, "not opened for reading" unless @readable
    if @pos < @string.length
      @string[@pos..-1].each_byte { |b| @pos += 1; yield b}
    end
    nil
  end

  def each_char
    return to_enum :each_char unless block_given?
    if $KCODE == "UTF-8"
      lookup = 7.downto(4)
      while c = read(1) do
        n = c[0]
        leftmost_zero_bit = lookup.find{|i| n[i].zero? }
        case leftmost_zero_bit
        when 7 # ASCII
          yield c
        when 6 # UTF 8 complementary characters
          next # Encoding error, ignore
        else
          more = read(6-leftmost_zero_bit)
          break unless more
          yield c+more
        end
      end
    else
      while s = read(1)
        yield s
      end
    end

    self
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

  alias_method :getbyte, :getc

  alias_method :readbyte, :readchar

end
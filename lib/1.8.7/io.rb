class IO
  def bytes
    to_enum :each_byte
  end

  def chars
    to_enum :each_char
  end

  ##
  # Executes the block for every line in ios, where
  # lines are separated by sep_string. ios must be
  # opened for reading or an IOError will be raised.
  #
  #  f = File.new("testfile")
  #  f.each {|line| puts "#{f.lineno}: #{line}" }
  # produces:
  #
  #  1: This is line one
  #  2: This is line two
  #  3: This is line three
  #  4: And so on...
  def each(sep=$/)
    return to_enum :each, sep unless block_given?
    ensure_open_and_readable

    sep = sep.to_str if sep
    while line = read_to_separator(sep)
      yield line
    end

    self
  end

  alias_method :each_line, :each

  def each_byte
    return to_enum :each_byte unless block_given?
    yield getbyte until eof?

    self
  end

  def each_char
    return to_enum :each_char unless block_given?
    ensure_open_and_readable
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

  def getbyte
    char = read 1
    return nil if char.nil?
    char[0]
  end

  def readbyte
    byte = getbyte
    raise EOFError, "end of file" unless bytes
    byte
  end
  
  def lines(*args)
    to_enum :each_line, *args
  end
end
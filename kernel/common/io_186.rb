# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class IO
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
    ensure_open_and_readable

    sep = sep.to_str if sep
    while line = read_to_separator(sep)
      yield line
    end

    self
  end

  alias_method :each_line, :each

  def each_byte
    yield getc until eof?

    self
  end
end
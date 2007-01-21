class IO
  def write(str)
    Ruby.primitive :io_write
    exc = IOError.new("Unable to write '#{str}' via #{self}")
    raise exc
  end
end

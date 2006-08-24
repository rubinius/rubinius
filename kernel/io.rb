
class IOError < Exception
end

class IO
  def puts(str)
    write str
    write "\n"
  end
  
  def write(str)
    Ruby.primitive :io_write
    exc = IOError.new("Unable to write '#{str}' via #{self}")
    raise exc
  end
  
  def read(size)
    Ruby.primitive :io_read
  end
end

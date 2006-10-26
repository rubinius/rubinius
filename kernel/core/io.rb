
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
  
  def close
    Ruby.primitive :io_close
    
    if self.descriptor == -1
      raise IOError.new("IO instance already closed")
    else
      raise IOError.new("Unable to close instance of IO")
    end
  end
  
  index_reader :descriptor, 0
  
  def closed?
    self.descriptor == -1
  end
  
  def self.create_pipe(lhs, rhs)
    Ruby.primitive :create_pipe
  end
  
  def self.pipe
    lhs = IO.allocate
    rhs = IO.allocate
    out = create_pipe(lhs, rhs)
    return [lhs, rhs]
  end
end

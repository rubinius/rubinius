class StandardError < Exception
end

class IOError < StandardError
end

class IO
  def puts(str)
    write "#{str}\n"
  end
  
  def <<(obj)
    write(obj.to_s)
    return self
  end

  def read(size=nil)
    if size
      buf = String.new(size)
      chan = Channel.new
      chan.send_on_readable self, buf
      out = chan.receive
      return out[1]
    else
      out = ""
      loop do
        chunk = read(32)
        return out unless chunk
        out << chunk
      end      
    end
  end
  
  def close
    Ruby.primitive :io_close
    
    if @descriptor == -1
      raise IOError.new("IO instance already closed")
    else
      raise IOError.new("Unable to close instance of IO")
    end
  end
  
  index_reader :descriptor, 0
  
  def closed?
    @descriptor == -1
  end
  
  # The current implementation does no buffering, so we're always
  # in sync mode.
  def sync=(v)
  end
  
  def sync
    true
  end
  
  def gets
    cur = read(1)
    return nil unless cur
    out = cur
    until out[-1] == 10
      cur = read(1)
      return out unless cur
      out << cur
    end
    
    return out    
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
  
  def reopen(other)
    Ruby.primitive :io_reopen
    raise ArgumentError, "only accepts an IO object"
  end
end

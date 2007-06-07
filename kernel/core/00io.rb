class StandardError < Exception
end

class IOError < StandardError
end

class IO
  
  def initialize(fd)
    @descriptor = fd
  end
  
  def inspect
    "#<#{self.class}:#{object_id.to_s(16)} fd=#{@descriptor}>"
  end
  
  def puts(str)
    write "#{str}\n"
  end
  
  def <<(obj)
    write(obj.to_s)
    return self
  end
  
  alias :print :<<

  def read(size=nil)
    if size
      buf = String.new(size)
      chan = Channel.new
      Scheduler.send_on_readable chan, self, buf, size
      chan.receive
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
  
  index_reader :descriptor, 1
  
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

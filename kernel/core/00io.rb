class StandardError < Exception
end

class IOError < StandardError
end

class IO
  
  def initialize(fd)
    @descriptor = fd
  end
  
  ivar_as_index :__ivars__ => 0, :descriptor => 1
  def __ivars__ ; @__ivars__  ; end
    
  def inspect
    "#<#{self.class}:#{object_id.to_s(16)} fd=#{@descriptor}>"
  end
  
  def puts(*args)
    if args.empty?
      write $/
    else
      args.each do |arg|
        str = arg.to_s

        write str
        write $/ unless str.suffix?($/)
      end
    end

    nil
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
    unless io_close
      raise IOError.new("Unable to close instance of IO")
    end
  end
  
  def descriptor
    @descriptor
  end
    
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
  
  def gets(sep=$/)
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
  
  def each(sep=$/)
    while line = gets(sep)
      yield line
    end
  end
  
  def self.pipe
    lhs = IO.allocate
    rhs = IO.allocate
    out = create_pipe(lhs, rhs)
    return [lhs, rhs]
  end
  
  private :io_close
end

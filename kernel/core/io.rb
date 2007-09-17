class StandardError < Exception
end

class IOError < StandardError
end

class IO
  
  BufferSize = 8096

  SEEK_CUR = 1
  SEEK_END = 2
  
  def initialize(fd)
    @descriptor = fd
  end
  
  ivar_as_index :__ivars__ => 0, :descriptor => 1, :buffer => 2, :mode => 3
  def __ivars__ ; @__ivars__  ; end
    
  def inspect
    "#<#{self.class}:#{object_id.to_s(16)}>"
  end
  
  def puts(*args)
    if args.empty?
      write DEFAULT_RECORD_SEPARATOR
    else
      args.each do |arg|
        str = arg.to_s

        write str
        write DEFAULT_RECORD_SEPARATOR unless str.suffix?(DEFAULT_RECORD_SEPARATOR)
      end
    end

    nil
  end
  
  def <<(obj)
    write(obj.to_s)
    return self
  end
  
  alias_method :print, :<<
    
  def sysread(size, buf=nil)
    buf = String.new(size) unless buf
    chan = Channel.new
    Scheduler.send_on_readable chan, self, buf, size
    raise EOFError if chan.receive.nil?
    return buf
  end
    
  alias_method :syswrite, :write

  def read(size=nil, buf=nil)
    if size
      buf = String.new(size) unless buf
      chan = Channel.new
      Scheduler.send_on_readable chan, self, buf, size
      return nil if chan.receive.nil?
      return buf
    else
      chunk = String.new(BufferSize)
      out = ""
      loop do
        chan = Channel.new
        Scheduler.send_on_readable chan, self, chunk, BufferSize
        
        return out if chan.receive.nil?
                
        out << chunk
      end
    end
  end
  
  def close
    unless io_close
      raise IOError "Unable to close instance of IO"
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

# depends on: class.rb

class IO

  ivar_as_index :__ivars__ => 0, :descriptor => 1, :buffer => 2, :mode => 3

  BufferSize = 8096

  class Buffer < String
    ivar_as_index :bytes => 0, :characters => 1, :encoding => 2, :data => 3, :hash => 4, :shared => 5

    def initialize(size)
      @data = ByteArray.new(size)
      @bytes = 0
      @characters = 0
      @encoding = :buffer

      @total = size - 1
      @channel = Channel.new
    end

    def process
      @channel.receive
    end

    attr_reader :channel

    def unused
      @total - @bytes + 1
    end

    def shift_front(count)
      count = @bytes if count > @bytes

      str = String.allocate
      str.initialize_from count, @data.fetch_bytes(0, count)

      rest = @bytes - count

      @data.move_bytes count, rest, 0

      @bytes -= count

      return str
    end

    def as_str
      str = String.allocate
      str.initialize_from @bytes, @data
      @data = ByteArray.new(@total)
      @bytes = 0
      return str
    end

    def full?
      @total == @bytes
    end

    def reset!
      @bytes = 0
    end

    def fill_from(io)
      Scheduler.send_on_readable @channel, io, self, unused()
      obj = @channel.receive
      if obj.kind_of? Class
        raise IOError, "error occured while filling buffer (#{obj})"
      end

      io.eof! unless obj

      return obj
    end

    def clip_to(reg)
      if m = reg.match(self)
        idx = m.end(0)
        return shift_front(idx)
      else
        nil
      end
    end
  end

  SEEK_SET = Rubinius::RUBY_CONFIG['rbx.platform.io.SEEK_SET']
  SEEK_CUR = Rubinius::RUBY_CONFIG['rbx.platform.io.SEEK_CUR']
  SEEK_END = Rubinius::RUBY_CONFIG['rbx.platform.io.SEEK_END']

  def initialize(fd)
    desc = Type.coerce_to fd, Fixnum, :to_int
    if desc < 0
      raise Errno::EBADF, "invalid descriptor"
    end

    setup(desc)
  end

  # Obtains a new duplicate descriptor for the current
  # one 
  #
  # Used internally by #dup and #clone. This is called
  # on the new object after it has been copied normally.
  def initialize_copy(original)
    @descriptor = Platform::POSIX.dup(@descriptor)
  end

  private :initialize_copy

  def setup(desc=nil)
    @descriptor = desc if desc
    @buffer = IO::Buffer.new(BufferSize)
    @eof = false
    @lineno = 0
  end

  attr_accessor :lineno

  def eof!
    @eof = true
  end

  def eof?
    @eof
  end

  def wait_til_readable
    chan = Channel.new
    Scheduler.send_on_readable chan, self, nil, nil
    chan.receive
  end

  def __ivars__ ; @__ivars__  ; end

  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)}>"
  end

  def fileno
    @descriptor
  end

  alias_method :to_i, :fileno

  def printf(fmt, *args)
    write YSprintf.new(fmt, *args).parse
  end
  
  def puts(*args)
    if args.empty?
      write DEFAULT_RECORD_SEPARATOR
    else
      args.each do |arg|
        if arg.nil?
          str = "nil"
        elsif RecursionGuard.inspecting?(arg)
          str = "[...]"
        elsif arg.kind_of?(Array)
          RecursionGuard.inspect(arg) do
            arg.each do |a|
              puts a
            end
          end
        else
          str = arg.to_s
        end

        if str
          write str
          write DEFAULT_RECORD_SEPARATOR unless str.suffix?(DEFAULT_RECORD_SEPARATOR)
        end
      end
    end

    nil
  end

  # Writes each given argument.to_s to the stream or $_ (the
  # result of last IO#gets) if called without arguments. Appends 
  # $\.to_s to output. Returns nil.
  def print(*args)
    if args.empty?
      write $_.to_s
    else
      args.each {|o| write o.to_s }
    end

    write $\.to_s
    nil
  end

  def <<(obj)
    write(obj.to_s)
    return self
  end

  def sysread(size, buf=nil)
    buf = String.new(size) unless buf
    chan = Channel.new
    Scheduler.send_on_readable chan, self, buf, size
    raise EOFError if chan.receive.nil?
    return buf
  end

  alias_method :readpartial, :sysread

  alias_method :syswrite, :write

  def breadall(output=nil)
    return "" if @eof

    buf = @buffer

    while true
      bytes = buf.fill_from(self)

      if !bytes or buf.full?
        if output
          output << buf
          buf.reset!
        else
          output = buf.as_str
        end
      end

      break unless bytes
    end

    return output
  end

  def read(size=nil, output=nil)
    return breadall(output) unless size

    return nil if @eof

    buf = @buffer

    # If we already have the data, return it fast.
    if size <= buf.size
      return buf.shift_front(size)
    end

    needed = size

    while true
      bytes = buf.fill_from(self)

      if bytes
        needed -= bytes
        done = (needed <= 0)
      else
        done = true
      end

      if done or buf.full?
        if output
          output << buf.shift_front(size)
          buf.reset!
        else
          output = buf.shift_front(size)
        end
      end

      break if done
    end

    return output
  end

  def seek(amount, whence=SEEK_SET)
    # Unseek the still buffered amount
    unless @buffer.empty?
      prim_seek -@buffer.size, SEEK_CUR
      @buffer.reset!
    end

    prim_seek amount, whence
  end

  def pos
    seek 0, SEEK_CUR
  end

  alias_method :tell, :pos

  def pos=(offset)
    seek offset, SEEK_SET
  end

  def rewind
    seek 0
    @lineno = 0
    @eof = false
    return 0
  end

  def close
    raise IOError, "Instance of IO already closed" if closed?
    io_close
  end

  def descriptor
    @descriptor
  end

  def closed?
    @descriptor == -1
  end

  # The current implementation does no write buffering, so we're always
  # in sync mode.
  def sync=(v)
  end

  def sync
    true
  end

  def flush
    true
  end

  def gets(sep=$/)
    @lineno += 1
    $_ = gets_helper(sep)
  end

  def readline(sep=$/)
    out = gets(sep)
    raise EOFError, "end of file" unless out
    return out
  end

  def each(sep=$/)
    while line = gets_helper(sep)
      yield line
    end
  end

  alias_method :each_line, :each

  # Several methods use similar rules for reading strings from IO, but
  # differ slightly. This helper is an extraction of the code.

  def gets_helper(sep=$/)
    return nil if @eof

    return breadall() unless sep

    buf = @buffer

    if sep.empty?
      return gets_stripped($/ + $/)
    end
    
    reg = /#{sep}/m

    if str = buf.clip_to(reg)
      return str
    end

    # Do an initial fill.
    return nil if !buf.fill_from(self) and buf.empty?

    output = nil
    while true
      if str = buf.clip_to(reg)
        if output
          return output + str
        else
          return str
        end
      end

      if !buf.fill_from(self)
        if buf.empty?
          rest = nil
        else
          rest = buf.as_str
        end

        if output
          if rest
            return output << buf.as_str
          else
            return output
          end
        else
          return rest
        end
      end

      if buf.full?
        if output
          output << buf
          buf.reset!
        else
          output = buf.as_str
        end
      end
    end
  end

  def gets_stripped(sep)
    buf = @buffer

    if m = /^\n+/m.match(buf)
      buf.shift_front(m.end(0)) if m.begin(0) == 0
    end

    str = gets_helper(sep)

    if m = /^\n+/m.match(buf)
      buf.shift_front(m.end(0)) if m.begin(0) == 0
    end

    return str
  end

  def readlines(sep=$/)
    ary = Array.new
    while line = gets(sep)
      ary << line
    end
    return ary
  end

  def self.readlines(name, sep_string = $/)
    io = File.open(StringValue(name), 'r')
    return if io.nil?

    begin
      io.readlines(sep_string)
    ensure
      io.close
    end
  end

  def self.foreach(name, sep_string = $/,&block)
    io = File.open(StringValue(name), 'r')
    sep = StringValue(sep_string)
    begin
      while(line = io.gets(sep))
        yield line
      end
    ensure
      io.close
    end
  end

  def self.pipe
    lhs = IO.allocate
    rhs = IO.allocate
    out = create_pipe(lhs, rhs)
    lhs.setup
    rhs.setup
    return [lhs, rhs]
  end

  def self.for_fd(fd)
    self.new(fd)
  end

  def self.open(*args)
    o = self.new(*args)

    return o unless block_given?

    begin
      yield o
    ensure
      o.close unless o.closed?
    end
  end

  def self.read(name, length = Undefined, offset = 0)
    name = StringValue(name)

    offset = Type.coerce_to(offset, Fixnum, :to_int)

    if offset < 0
      raise Errno::EINVAL, "offset must not be negative"
    end

    unless length.equal?(Undefined)
      length = Type.coerce_to(length, Fixnum, :to_int)

      if length < 0
        raise ArgumentError, "length must not be negative"
      end
    end

    File.open(name) do |f|
      f.seek(offset) unless offset.zero?

      if length.equal?(Undefined)
        f.read
      else
        f.read(length)
      end
    end
  end

  private :io_close
end

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

  module Constants
    SEEK_SET = Rubinius::RUBY_CONFIG['rbx.platform.io.SEEK_SET']
    SEEK_CUR = Rubinius::RUBY_CONFIG['rbx.platform.io.SEEK_CUR']
    SEEK_END = Rubinius::RUBY_CONFIG['rbx.platform.io.SEEK_END']

    RDONLY   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_RDONLY']
    WRONLY   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_WRONLY']
    RDWR     = Rubinius::RUBY_CONFIG['rbx.platform.file.O_RDWR']
  
    CREAT    = Rubinius::RUBY_CONFIG['rbx.platform.file.O_CREAT']
    EXCL     = Rubinius::RUBY_CONFIG['rbx.platform.file.O_EXCL']
    NOCTTY   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_NOCTTY']
    TRUNC    = Rubinius::RUBY_CONFIG['rbx.platform.file.O_TRUNC']
    APPEND   = Rubinius::RUBY_CONFIG['rbx.platform.file.O_APPEND']
    NONBLOCK = Rubinius::RUBY_CONFIG['rbx.platform.file.O_NONBLOCK']
    SYNC     = Rubinius::RUBY_CONFIG['rbx.platform.file.O_SYNC']
  
    S_IRUSR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IRUSR']
    S_IWUSR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IWUSR']
    S_IXUSR  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IXUSR']
    S_IRGRP  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IRGRP']
    S_IWGRP  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IWGRP']
    S_IXGRP  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IXGRP']
    S_IROTH  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IROTH']
    S_IWOTH  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IWOTH']
    S_IXOTH  = Rubinius::RUBY_CONFIG['rbx.platform.file.S_IXOTH']

    # TODO: these flags should probably be imported from Platform
    LOCK_SH  = 0x01
    LOCK_EX  = 0x02
    LOCK_NB  = 0x04
    LOCK_UN  = 0x08
    BINARY   = 0x04
  end
  include Constants
  
  def self.select(read_array, write_array = nil, error_array = nil,
                  timeout = nil)
    chan = Channel.new

    if read_array then
      read_array.each do |readable|
        Scheduler.send_on_readable chan, readable, nil, nil
      end
    end

    raise NonImplementedError, "write_array is not supported" if write_array
    raise NonImplementedError, "error_array is not supported" if error_array

    # HACK can't do this yet
    #if write_array then
    #  write_array.each do |writable|
    #    Scheduler.send_on_writable chan, writable, nil, nil
    #  end
    #end
    #
    #if errore_array then
    #  errore_array.each do |errorable|
    #    Scheduler.send_on_error chan, errorable, nil, nil
    #  end
    #end

    Scheduler.send_in_microseconds chan, (timeout * 1_000_000).to_i if timeout

    value = chan.receive

    return nil if value == 1 # timeout

    io = read_array.find { |readable| readable.fileno == value }

    return nil if io.nil?

    [[io], [], []]
  end

  def initialize(fd)
    desc = Type.coerce_to fd, Fixnum, :to_int
    if desc < 0
      raise Errno::EBADF, "invalid descriptor"
    end

    setup(desc)
  end

  # Obtains a new duplicate descriptor for the current one.
  #
  # Used internally by #dup and #clone. This is called on the new object after
  # it has been copied normally.
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

  alias_method :isatty, :tty?

  def self.popen(str, mode="r")
    raise "TODO make this support more than r" if mode != "r"
    
    if str == "+-+" and !block_given?
      raise ArgumentError, "this mode requires a block currently"
    end

    pa_read, ch_write = IO.pipe
    
    pid = Process.fork

    if pid
      ch_write.close
      # See bottom for definition
      rp = BidirectionalPipe.new(pid, pa_read, nil)
      if block_given?
        begin
          yield rp
        ensure
          pa_read.close
        end
      else
        return rp
      end
    else
      pa_read.close
      STDOUT.reopen ch_write
      if str == "+-+"
        yield nil
      else
        Process.replace "/bin/sh", ["sh", "-c", str]
      end
    end
  end

  def binmode
    # HACK what to do?
  end

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
    write Sprintf.new(fmt, *args).parse
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

  alias_method :prim_write, :write
  
  def write(data)
    # If we have buffered data, rewind.
    unless @buffer.empty?
      seek 0, SEEK_CUR
    end

    data = String data

    prim_write(data)
  end
  
  alias_method :syswrite, :write

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
    io_close or raise SystemCallError, "Invalid file descriptor"
  end

  def descriptor
    @descriptor
  end

  def sysseek(amount, whence=SEEK_SET)
    Platform::POSIX.lseek(@descriptor, amount, whence)
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

  def self.sysopen(path, mode = "r", perm = 0666)
    if mode.kind_of?(String)
      mode = parse_mode(mode)
    end

    return open_with_mode(path, mode, perm)
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

  def fcntl(command, arg)
    if arg.kind_of? Fixnum then
      IO._fcntl_int descriptor, command, arg
    else
      raise NotImplementedError, "cannot handle #{arg.class}"
    end
  end

  def self.parse_mode(mode)
    ret = 0

    case mode[0]
    when ?r
      ret |= RDONLY
    when ?w
      ret |= WRONLY | CREAT | TRUNC
    when ?a
      ret |= WRONLY | CREAT | APPEND
    else
      raise ArgumentError, "invalid mode -- #{mode}"
    end

    return ret if mode.length == 1

    case mode[1]
    when ?+
      ret &= ~(RDONLY | WRONLY)
      ret |= RDWR
    when ?b
      ret |= BINARY
    else
      raise ArgumentError, "invalid mode -- #{mode}"
    end
  
    return ret if mode.length == 2

    case mode[2]
    when ?+
      ret &= ~(RDONLY | WRONLY)
      ret |= RDWR
    when ?b
      ret |= BINARY
    else
      raise ArgumentError, "invalid mode -- #{mode}"
    end

    ret
  end
  
  private :io_close

  def self.after_loaded
    attach_function "fcntl", :_fcntl_int, [:int, :int, :int], :int
  end

end

class BidirectionalPipe < IO
  def initialize(pid, read, write)
    super(read.fileno)
    @pid = pid
    @write = write
  end
  
  def pid
    @pid
  end
  
  def <<(str)
    @write << str
  end
  
  alias_method :write, :<<
  
  def syswrite(str)
    @write.syswrite str
  end
end

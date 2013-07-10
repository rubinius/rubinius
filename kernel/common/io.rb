# -*- encoding: us-ascii -*-

class IO
  FFI = Rubinius::FFI

  include Enumerable

  # Import platform constants

  SEEK_SET = Rubinius::Config['rbx.platform.io.SEEK_SET']
  SEEK_CUR = Rubinius::Config['rbx.platform.io.SEEK_CUR']
  SEEK_END = Rubinius::Config['rbx.platform.io.SEEK_END']

  # InternalBuffer provides a sliding window into a region of bytes.
  # The buffer is filled to the +used+ indicator, which is
  # always less than or equal to +total+. As bytes are taken
  # from the buffer, the +start+ indicator is incremented by
  # the number of bytes taken. Once +start+ == +used+, the
  # buffer is +empty?+ and needs to be refilled.
  #
  # This description should be independent of the "direction"
  # in which the buffer is used. As a read buffer, +fill_from+
  # appends at +used+, but not exceeding +total+. When +used+
  # equals total, no additional bytes will be filled until the
  # buffer is emptied.
  #
  # As a write buffer, +empty_to+ removes bytes from +start+ up
  # to +used+. When +start+ equals +used+, no additional bytes
  # will be emptied until the buffer is filled.
  #
  # IO presents a stream of input. Buffer presents buckets of
  # input. IO's task is to chain the buckets so the user sees
  # a stream. IO explicitly requests that the buffer be filled
  # (on input) and then determines how much of the input to take
  # (e.g. by looking for a separator or collecting a certain
  # number of bytes). Buffer decides whether or not to go to the
  # source for more data or just present what is already in the
  # buffer.
  class InternalBuffer

    attr_reader :total
    attr_reader :start
    attr_reader :used
    attr_reader :channel

    ##
    # Returns +true+ if the buffer can be filled.
    def empty?
      @start == @used
    end

    ##
    # Returns +true+ if the buffer is empty and cannot be filled further.
    def exhausted?
      @eof and empty?
    end

    ##
    # A request to the buffer to have data. The buffer decides whether
    # existing data is sufficient, or whether to read more data from the
    # +IO+ instance. Any new data causes this method to return.
    #
    # Returns the number of bytes in the buffer.
    def fill_from(io, skip = nil)
      Rubinius.synchronize(self) do
        empty_to io
        discard skip if skip

        return size unless empty?

        reset!

        if fill(io) < 0
          raise IOError, "error occurred while filling buffer (#{obj})"
        end

        if @used == 0
          io.eof!
          @eof = true
        end

        return size
      end
    end

    def empty_to(io)
      return 0 if @write_synced or empty?
      @write_synced = true

      io.prim_write(String.from_bytearray(@storage, @start, size))
      reset!

      return size
    end

    ##
    # Advances the beginning-of-buffer marker past any number
    # of contiguous characters == +skip+. For example, if +skip+
    # is ?\n and the buffer contents are "\n\n\nAbc...", the
    # start marker will be positioned on 'A'.
    def discard(skip)
      while @start < @used
        break unless @storage[@start] == skip
        @start += 1
      end
    end

    ##
    # Returns the number of bytes to fetch from the buffer up-to-
    # and-including +pattern+. Returns +nil+ if pattern is not found.
    def find(pattern, discard = nil)
      if count = @storage.locate(pattern, @start, @used)
        count - @start
      end
    end

    ##
    # Returns +true+ if the buffer is filled to capacity.
    def full?
      @total == @used
    end

    def inspect # :nodoc:
      "#<IO::InternalBuffer:0x%x total=%p start=%p used=%p data=%p>" % [
        object_id, @total, @start, @used, @storage
      ]
    end

    ##
    # Resets the buffer state so the buffer can be filled again.
    def reset!
      @start = @used = 0
      @eof = false
      @write_synced = true
    end

    def write_synced?
      @write_synced
    end

    def unseek!(io)
      Rubinius.synchronize(self) do
        # Unseek the still buffered amount
        return unless write_synced?
        io.prim_seek @start - @used, IO::SEEK_CUR unless empty?
        reset!
      end
    end

    ##
    # Returns +count+ bytes from the +start+ of the buffer as a new String.
    # If +count+ is +nil+, returns all available bytes in the buffer.
    def shift(count=nil)
      Rubinius.synchronize(self) do
        total = size
        total = count if count and count < total

        str = String.from_bytearray @storage, @start, total
        @start += total

        str
      end
    end

    ##
    # Returns one Fixnum as the start byte.
    def getbyte(io)
      return if size == 0 and fill_from(io) == 0

      Rubinius.synchronize(self) do
        byte = @storage[@start]
        @start += 1
        byte
      end
    end

    ##
    # Prepends the byte +chr+ to the internal buffer, so that future
    # reads will return it.
    def put_back(chr)
      # A simple case, which is common and can be done efficiently
      if @start > 0
        @start -= 1
        @storage[@start] = chr
      else
        @storage = @storage.prepend(chr.chr)
        @start = 0
        @total = @storage.size
        @used += 1
      end
    end

    ##
    # Returns the number of bytes available in the buffer.
    def size
      @used - @start
    end

    ##
    # Returns the number of bytes of capacity remaining in the buffer.
    # This is the number of additional bytes that can be added to the
    # buffer before it is full.
    def unused
      @total - @used
    end
  end

  attr_accessor :descriptor
  attr_accessor :mode

  def self.open(*args)
    io = new(*args)

    return io unless block_given?

    begin
      yield io
    ensure
      begin
        io.close unless io.closed?
      rescue StandardError
        # nothing, just swallow them.
      end
    end
  end

  def self.parse_mode(mode)
    return mode if Rubinius::Type.object_kind_of? mode, Integer

    mode = StringValue(mode)

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
    when ?:
      warn("encoding options not supported in 1.8")
      return ret
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
    when ?:
      warn("encoding options not supported in 1.8")
      return ret
    else
      raise ArgumentError, "invalid mode -- #{mode}"
    end

    ret
  end

  #
  # +select+ examines the IO object Arrays that are passed in
  # as +readables+, +writables+, and +errorables+ to see if any
  # of their descriptors are ready for reading, are ready for
  # writing, or have an exceptions pending respectively. An IO
  # may appear in more than one of the sets. Any of the three
  # sets may be +nil+ if you are not interested in those events.
  #
  # If +timeout+ is not nil, it specifies the number of seconds
  # to wait for events (maximum.) The number may be fractional,
  # conceptually up to a microsecond resolution.
  #
  # A +timeout+ of 0 indicates that each descriptor should be
  # checked once only, effectively polling the sets.
  #
  # Leaving the +timeout+ to +nil+ causes +select+ to block
  # infinitely until an event transpires.
  #
  # If the timeout expires without events, +nil+ is returned.
  # Otherwise, an [readable, writable, errors] Array of Arrays
  # is returned, only, with the IO objects that have events.
  #
  # @compatibility  MRI 1.8 and 1.9 require the +readables+ Array,
  #                 Rubinius does not.
  #
  def self.select(readables=nil, writables=nil, errorables=nil, timeout=nil)
    if timeout
      unless Rubinius::Type.object_kind_of? timeout, Numeric
        raise TypeError, "Timeout must be numeric"
      end

      raise ArgumentError, 'timeout must be positive' if timeout < 0

      # Microseconds, rounded down
      timeout = Integer(timeout * 1_000_000)
    end

    if readables
      readables =
        Rubinius::Type.coerce_to(readables, Array, :to_ary).map do |obj|
          if obj.kind_of? IO
            raise IOError, "closed stream" if obj.closed?
            return [[obj],[],[]] unless obj.buffer_empty?
            obj
          else
            io = Rubinius::Type.coerce_to(obj, IO, :to_io)
            raise IOError, "closed stream" if io.closed?
            [obj, io]
          end
        end
    end

    if writables
      writables =
        Rubinius::Type.coerce_to(writables, Array, :to_ary).map do |obj|
          if obj.kind_of? IO
            raise IOError, "closed stream" if obj.closed?
            obj
          else
            io = Rubinius::Type.coerce_to(obj, IO, :to_io)
            raise IOError, "closed stream" if io.closed?
            [obj, io]
          end
        end
    end

    if errorables
      errorables =
        Rubinius::Type.coerce_to(errorables, Array, :to_ary).map do |obj|
          if obj.kind_of? IO
            raise IOError, "closed stream" if obj.closed?
            obj
          else
            io = Rubinius::Type.coerce_to(obj, IO, :to_io)
            raise IOError, "closed stream" if io.closed?
            [obj, io]
          end
        end
    end

    IO.select_primitive(readables, writables, errorables, timeout)
  end

  ##
  # Opens the given path, returning the underlying file descriptor as a Fixnum.
  #  IO.sysopen("testfile")   #=> 3
  def self.sysopen(path, mode = nil, perm = nil)
    path = Rubinius::Type.coerce_to_path path
    mode = parse_mode(mode || "r")
    perm ||= 0666

    open_with_mode path, mode, perm
  end

  #
  # Internally associate +io+ with the given descriptor.
  #
  # The +mode+ will be checked and set as the current mode if
  # the underlying descriptor allows it.
  #
  # The +sync+ attribute will also be set.
  #
  def self.setup(io, fd, mode=nil, sync=false)
    cur_mode = FFI::Platform::POSIX.fcntl(fd, F_GETFL, 0)
    Errno.handle if cur_mode < 0

    cur_mode &= ACCMODE

    if mode
      mode = parse_mode(mode)
      mode &= ACCMODE

      if (cur_mode == RDONLY or cur_mode == WRONLY) and mode != cur_mode
        raise Errno::EINVAL, "Invalid new mode for existing descriptor #{fd}"
      end
    end

    io.descriptor = fd
    io.mode       = mode || cur_mode
    io.sync       = !!sync
    io.sync     ||= STDOUT.fileno == fd if STDOUT.respond_to?(:fileno)
    io.sync     ||= STDERR.fileno == fd if STDERR.respond_to?(:fileno)
  end

  def self.max_open_fd
    @max_open_fd.get
  end

  ##
  # Obtains a new duplicate descriptor for the current one.
  def initialize_copy(original) # :nodoc:
    @descriptor = FFI::Platform::POSIX.dup(@descriptor)
  end

  private :initialize_copy

  # Used to find out if there is buffered data available.
  def buffer_empty?
    @ibuffer.empty?
  end

  def <<(obj)
    write(obj.to_s)
    return self
  end

  ##
  # Closes the read end of a duplex I/O stream (i.e., one
  # that contains both a read and a write stream, such as
  # a pipe). Will raise an IOError if the stream is not duplexed.
  #
  #  f = IO.popen("/bin/sh","r+")
  #  f.close_read
  #  f.readlines
  # produces:
  #
  #  prog.rb:3:in `readlines': not opened for reading (IOError)
  #   from prog.rb:3
  def close_read
    if @mode == WRONLY || @mode == RDWR
      raise IOError, 'closing non-duplex IO for reading'
    end
    close
  end

  ##
  # Closes the write end of a duplex I/O stream (i.e., one
  # that contains both a read and a write stream, such as
  # a pipe). Will raise an IOError if the stream is not duplexed.
  #
  #  f = IO.popen("/bin/sh","r+")
  #  f.close_write
  #  f.print "nowhere"
  # produces:
  #
  #  prog.rb:3:in `write': not opened for writing (IOError)
  #   from prog.rb:3:in `print'
  #   from prog.rb:3
  def close_write
    if @mode == RDONLY || @mode == RDWR
      raise IOError, 'closing non-duplex IO for writing'
    end
    close
  end

  ##
  # Returns true if ios is completely closed (for duplex
  # streams, both reader and writer), false otherwise.
  #
  #  f = File.new("testfile")
  #  f.close         #=> nil
  #  f.closed?       #=> true
  #  f = IO.popen("/bin/sh","r+")
  #  f.close_write   #=> nil
  #  f.closed?       #=> false
  #  f.close_read    #=> nil
  #  f.closed?       #=> true
  def closed?
    @descriptor == -1
  end

  def dup
    ensure_open
    super
  end

  def each_byte
    return to_enum(:each_byte) unless block_given?

    yield getbyte until eof?

    self
  end

  alias_method :bytes, :each_byte

  ##
  # Set the pipe so it is at the end of the file
  def eof!
    @eof = true
  end

  ##
  # Returns true if ios is at end of file that means
  # there are no more data to read. The stream must be
  # opened for reading or an IOError will be raised.
  #
  #  f = File.new("testfile")
  #  dummy = f.readlines
  #  f.eof   #=> true
  # If ios is a stream such as pipe or socket, IO#eof?
  # blocks until the other end sends some data or closes it.
  #
  #  r, w = IO.pipe
  #  Thread.new { sleep 1; w.close }
  #  r.eof?  #=> true after 1 second blocking
  #
  #  r, w = IO.pipe
  #  Thread.new { sleep 1; w.puts "a" }
  #  r.eof?  #=> false after 1 second blocking
  #
  #  r, w = IO.pipe
  #  r.eof?  # blocks forever
  #
  # Note that IO#eof? reads data to a input buffer.
  # So IO#sysread doesn't work with IO#eof?.
  def eof?
    ensure_open_and_readable
    @ibuffer.fill_from self unless @ibuffer.exhausted?
    @eof and @ibuffer.exhausted?
  end

  alias_method :eof, :eof?

  def ensure_open_and_readable
    ensure_open
    write_only = @mode & ACCMODE == WRONLY
    raise IOError, "not opened for reading" if write_only
  end

  def ensure_open_and_writable
    ensure_open
    read_only = @mode & ACCMODE == RDONLY
    raise IOError, "not opened for writing" if read_only
  end

  ##
  # Provides a mechanism for issuing low-level commands to
  # control or query file-oriented I/O streams. Arguments
  # and results are platform dependent. If arg is a number,
  # its value is passed directly. If it is a string, it is
  # interpreted as a binary sequence of bytes (Array#pack
  # might be a useful way to build this string). On Unix
  # platforms, see fcntl(2) for details. Not implemented on all platforms.
  def fcntl(command, arg=0)
    ensure_open

    if !arg
      arg = 0
    elsif arg == true
      arg = 1
    elsif arg.kind_of? String
      raise NotImplementedError, "cannot handle String"
    else
      arg = Rubinius::Type.coerce_to arg, Fixnum, :to_int
    end

    command = Rubinius::Type.coerce_to command, Fixnum, :to_int
    FFI::Platform::POSIX.fcntl descriptor, command, arg
  end

  ##
  # Provides a mechanism for issuing low-level commands to
  # control or query file-oriented I/O streams. Arguments
  # and results are platform dependent. If arg is a number,
  # its value is passed directly. If it is a string, it is
  # interpreted as a binary sequence of bytes (Array#pack
  # might be a useful way to build this string). On Unix
  # platforms, see fcntl(2) for details. Not implemented on all platforms.
  def ioctl(command, arg=0)
    ensure_open

    if !arg
      real_arg = 0
    elsif arg == true
      real_arg = 1
    elsif arg.kind_of? String
      # This could be faster.
      buffer_size = arg.length
      # On BSD and Linux, we could read the buffer size out of the ioctl value.
      # Most Linux ioctl codes predate the convention, so a fallback like this
      # is still necessary.
      buffer_size = 4096 if buffer_size < 4096
      buffer = FFI::MemoryPointer.new buffer_size
      buffer.write_string arg, arg.length
      real_arg = buffer.address
    else
      real_arg = Rubinius::Type.coerce_to arg, Fixnum, :to_int
    end

    command = Rubinius::Type.coerce_to command, Fixnum, :to_int
    ret = FFI::Platform::POSIX.ioctl descriptor, command, real_arg
    Errno.handle if ret < 0
    if arg.kind_of?(String)
      arg.replace buffer.read_string(buffer_size)
      buffer.free
    end
    ret
  end

  ##
  # Returns an integer representing the numeric file descriptor for ios.
  #
  #  $stdin.fileno    #=> 0
  #  $stdout.fileno   #=> 1
  def fileno
    ensure_open
    @descriptor
  end

  alias_method :to_i, :fileno

  ##
  # Flushes any buffered data within ios to the underlying
  # operating system (note that this is Ruby internal
  # buffering only; the OS may buffer the data as well).
  #
  #  $stdout.print "no newline"
  #  $stdout.flush
  # produces:
  #
  #  no newline
  def flush
    ensure_open
    @ibuffer.empty_to self
    self
  end

  ##
  # Immediately writes all buffered data in ios to disk. Returns
  # nil if the underlying operating system does not support fsync(2).
  # Note that fsync differs from using IO#sync=. The latter ensures
  # that data is flushed from Ruby's buffers, but does not guarantee
  # that the underlying operating system actually writes it to disk.
  def fsync
    flush

    err = FFI::Platform::POSIX.fsync @descriptor

    Errno.handle 'fsync(2)' if err < 0

    err
  end

  ##
  # Gets the next 8-bit byte (0..255) from ios.
  # Returns nil if called at end of file.
  #
  #  f = File.new("testfile")
  #  f.getc   #=> 84
  #  f.getc   #=> 104
  def getc
    ensure_open

    return @ibuffer.getchar(self)
  end

  ##
  # Returns the current line number in ios. The
  # stream must be opened for reading. lineno
  # counts the number of times gets is called,
  # rather than the number of newlines encountered.
  # The two values will differ if gets is called with
  # a separator other than newline. See also the $. variable.
  #
  #  f = File.new("testfile")
  #  f.lineno   #=> 0
  #  f.gets     #=> "This is line one\n"
  #  f.lineno   #=> 1
  #  f.gets     #=> "This is line two\n"
  #  f.lineno   #=> 2
  def lineno
    ensure_open

    @lineno
  end

  ##
  # Manually sets the current line number to the
  # given value. $. is updated only on the next read.
  #
  #  f = File.new("testfile")
  #  f.gets                     #=> "This is line one\n"
  #  $.                         #=> 1
  #  f.lineno = 1000
  #  f.lineno                   #=> 1000
  #  $. # lineno of last read   #=> 1
  #  f.gets                     #=> "This is line two\n"
  #  $. # lineno of last read   #=> 1001
  def lineno=(line_number)
    ensure_open

    raise TypeError if line_number.nil?

    @lineno = Integer(line_number)
  end

  ##
  # FIXME
  # Returns the process ID of a child process
  # associated with ios. This will be set by IO::popen.
  #
  #  pipe = IO.popen("-")
  #  if pipe
  #    $stderr.puts "In parent, child pid is #{pipe.pid}"
  #  else
  #    $stderr.puts "In child, pid is #{$$}"
  #  end
  # produces:
  #
  #  In child, pid is 26209
  #  In parent, child pid is 26209
  def pid
    raise IOError, 'closed stream' if closed?
    @pid
  end

  attr_writer :pid

  ##
  #
  def pos
    flush
    @ibuffer.unseek! self

    prim_seek 0, SEEK_CUR
  end

  alias_method :tell, :pos

  ##
  # Seeks to the given position (in bytes) in ios.
  #
  #  f = File.new("testfile")
  #  f.pos = 17
  #  f.gets   #=> "This is line two\n"
  def pos=(offset)
    seek offset, SEEK_SET
  end

  ##
  # Writes each given argument.to_s to the stream or $_ (the result of last
  # IO#gets) if called without arguments. Appends $\.to_s to output. Returns
  # nil.
  def print(*args)
    if args.empty?
      write $_.to_s
    else
      args.each { |o| write o.to_s }
    end

    write $\.to_s
    nil
  end

  ##
  # Formats and writes to ios, converting parameters under
  # control of the format string. See Kernel#sprintf for details.
  def printf(fmt, *args)
    fmt = StringValue(fmt)
    write ::Rubinius::Sprinter.get(fmt).call(*args)
  end

  ##
  # If obj is Numeric, write the character whose code is obj,
  # otherwise write the first character of the string
  # representation of obj to ios.
  #
  #  $stdout.putc "A"
  #  $stdout.putc 65
  # produces:
  #
  #  AA
  def putc(obj)
    if Rubinius::Type.object_kind_of? obj, String
      write obj.substring(0, 1)
    else
      byte = Rubinius::Type.coerce_to(obj, Integer, :to_int) & 0xff
      write byte.chr
    end

    return obj
  end

  ##
  # Reads all input until +#eof?+ is true. Returns the input read.
  # If the buffer is already exhausted, returns +""+.
  def read_all
    str = ""
    until @ibuffer.exhausted?
      @ibuffer.fill_from self
      str << @ibuffer.shift
    end

    str
  end

  private :read_all

  # defined in bootstrap, used here.
  private :read_if_available

  ##
  # Reads at most maxlen bytes from ios using read(2) system
  # call after O_NONBLOCK is set for the underlying file descriptor.
  #
  # If the optional outbuf argument is present, it must reference
  # a String, which will receive the data.
  #
  # read_nonblock just calls read(2). It causes all errors read(2)
  # causes: EAGAIN, EINTR, etc. The caller should care such errors.
  #
  # read_nonblock causes EOFError on EOF.
  #
  # If the read buffer is not empty, read_nonblock reads from the
  # buffer like readpartial. In this case, read(2) is not called.
  def read_nonblock(size, buffer=nil)
    raise ArgumentError, "illegal read size" if size < 0
    ensure_open

    buffer = StringValue buffer if buffer

    if @ibuffer.size > 0
      return @ibuffer.shift(size)
    end

    if str = read_if_available(size)
      buffer.replace(str) if buffer
      return str
    else
      raise EOFError, "stream closed"
    end
  end

  ##
  # Reads a character as with IO#getc, but raises an EOFError on end of file.
  def readchar
    char = getc
    raise EOFError, 'end of file reached' unless char
    char
  end

  def readbyte
    byte = getbyte
    raise EOFError, "end of file reached" unless byte
    raise EOFError, "end of file" unless bytes
    byte
  end

  ##
  # Reads a line as with IO#gets, but raises an EOFError on end of file.
  def readline(sep=$/)
    out = gets(sep)
    raise EOFError, "end of file" unless out
    return out
  end

  ##
  # Reads all of the lines in ios, and returns them in an array.
  # Lines are separated by the optional sep_string. If sep_string
  # is nil, the rest of the stream is returned as a single record.
  # The stream must be opened for reading or an IOError will be raised.
  #
  #  f = File.new("testfile")
  #  f.readlines[0]   #=> "This is line one\n"
  def readlines(sep=$/)
    sep = StringValue sep if sep

    old_line = $_
    ary = Array.new
    while line = gets(sep)
      ary << line
    end
    $_ = old_line

    ary
  end

  ##
  # Reads at most maxlen bytes from the I/O stream. It blocks
  # only if ios has no data immediately available. It doesn‘t
  # block if some data available. If the optional outbuf argument
  # is present, it must reference a String, which will receive the
  # data. It raises EOFError on end of file.
  #
  # readpartial is designed for streams such as pipe, socket, tty,
  # etc. It blocks only when no data immediately available. This
  # means that it blocks only when following all conditions hold.
  #
  # the buffer in the IO object is empty.
  # the content of the stream is empty.
  # the stream is not reached to EOF.
  # When readpartial blocks, it waits data or EOF on the stream.
  # If some data is reached, readpartial returns with the data.
  # If EOF is reached, readpartial raises EOFError.
  #
  # When readpartial doesn‘t blocks, it returns or raises immediately.
  # If the buffer is not empty, it returns the data in the buffer.
  # Otherwise if the stream has some content, it returns the data in
  # the stream. Otherwise if the stream is reached to EOF, it raises EOFError.
  #
  #  r, w = IO.pipe           #               buffer          pipe content
  #  w << "abc"               #               ""              "abc".
  #  r.readpartial(4096)      #=> "abc"       ""              ""
  #  r.readpartial(4096)      # blocks because buffer and pipe is empty.
  #
  #  r, w = IO.pipe           #               buffer          pipe content
  #  w << "abc"               #               ""              "abc"
  #  w.close                  #               ""              "abc" EOF
  #  r.readpartial(4096)      #=> "abc"       ""              EOF
  #  r.readpartial(4096)      # raises EOFError
  #
  #  r, w = IO.pipe           #               buffer          pipe content
  #  w << "abc\ndef\n"        #               ""              "abc\ndef\n"
  #  r.gets                   #=> "abc\n"     "def\n"         ""
  #  w << "ghi\n"             #               "def\n"         "ghi\n"
  #  r.readpartial(4096)      #=> "def\n"     ""              "ghi\n"
  #  r.readpartial(4096)      #=> "ghi\n"     ""              ""
  # Note that readpartial behaves similar to sysread. The differences are:
  #
  # If the buffer is not empty, read from the buffer instead
  # of "sysread for buffered IO (IOError)".
  # It doesn‘t cause Errno::EAGAIN and Errno::EINTR. When readpartial
  # meets EAGAIN and EINTR by read system call, readpartial retry the system call.
  # The later means that readpartial is nonblocking-flag insensitive. It
  # blocks on the situation IO#sysread causes Errno::EAGAIN as if the fd is blocking mode.
  def readpartial(size, buffer=nil)
    raise ArgumentError, 'negative string size' unless size >= 0
    ensure_open

    if buffer
      buffer = StringValue(buffer)

      buffer.shorten! buffer.size

      return buffer if size == 0

      if @ibuffer.size > 0
        data = @ibuffer.shift(size)
      else
        data = sysread(size)
      end

      buffer.replace(data)

      return buffer
    else
      return "" if size == 0

      if @ibuffer.size > 0
        return @ibuffer.shift(size)
      end

      return sysread(size)
    end
  end

  ##
  # Reassociates ios with the I/O stream given in other_IO or to
  # a new stream opened on path. This may dynamically change the
  # actual class of this stream.
  #
  #  f1 = File.new("testfile")
  #  f2 = File.new("testfile")
  #  f2.readlines[0]   #=> "This is line one\n"
  #  f2.reopen(f1)     #=> #<File:testfile>
  #  f2.readlines[0]   #=> "This is line one\n"
  def reopen(other, mode=undefined)
    if other.respond_to?(:to_io)
      flush

      if other.kind_of? IO
        io = other
      else
        io = other.to_io
        unless io.kind_of? IO
          raise TypeError, "#to_io must return an instance of IO"
        end
      end

      io.ensure_open
      io.reset_buffering

      reopen_io io
      Rubinius::Unsafe.set_class self, io.class
      if io.respond_to?(:path)
        @path = io.path
      end
    else
      flush unless closed?

      # If a mode isn't passed in, use the mode that the IO is already in.
      if undefined.equal? mode
        mode = @mode
        # If this IO was already opened for writing, we should
        # create the target file if it doesn't already exist.
        if (mode & RDWR == RDWR) || (mode & WRONLY == WRONLY)
          mode |= CREAT
        end
      else
        mode = IO.parse_mode(mode)
      end

      reopen_path Rubinius::Type.coerce_to_path(other), mode
      seek 0, SEEK_SET
    end

    self
  end

  ##
  # Internal method used to reset the state of the buffer, including the
  # physical position in the stream.
  def reset_buffering
    @ibuffer.unseek! self
  end

  ##
  # Positions ios to the beginning of input, resetting lineno to zero.
  #
  #  f = File.new("testfile")
  #  f.readline   #=> "This is line one\n"
  #  f.rewind     #=> 0
  #  f.lineno     #=> 0
  #  f.readline   #=> "This is line one\n"
  def rewind
    seek 0
    @lineno = 0
    return 0
  end

  ##
  # Seeks to a given offset +amount+ in the stream according to the value of whence:
  #
  # IO::SEEK_CUR  | Seeks to _amount_ plus current position
  # --------------+----------------------------------------------------
  # IO::SEEK_END  | Seeks to _amount_ plus end of stream (you probably
  #               | want a negative value for _amount_)
  # --------------+----------------------------------------------------
  # IO::SEEK_SET  | Seeks to the absolute location given by _amount_
  # Example:
  #
  #  f = File.new("testfile")
  #  f.seek(-13, IO::SEEK_END)   #=> 0
  #  f.readline                  #=> "And so on...\n"
  def seek(amount, whence=SEEK_SET)
    flush

    @ibuffer.unseek! self
    @eof = false

    prim_seek Integer(amount), whence

    return 0
  end

  ##
  # Returns status information for ios as an object of type File::Stat.
  #
  #  f = File.new("testfile")
  #  s = f.stat
  #  "%o" % s.mode   #=> "100644"
  #  s.blksize       #=> 4096
  #  s.atime         #=> Wed Apr 09 08:53:54 CDT 2003
  def stat
    ensure_open

    File::Stat.fstat @descriptor
  end

  ##
  # Returns the current "sync mode" of ios. When sync mode is true,
  # all output is immediately flushed to the underlying operating
  # system and is not buffered by Ruby internally. See also IO#fsync.
  #
  #  f = File.new("testfile")
  #  f.sync   #=> false
  def sync
    ensure_open
    @sync == true
  end

  ##
  # Sets the "sync mode" to true or false. When sync mode is true,
  # all output is immediately flushed to the underlying operating
  # system and is not buffered internally. Returns the new state.
  # See also IO#fsync.
  def sync=(v)
    ensure_open
    @sync = !!v
  end

  ##
  # Reads integer bytes from ios using a low-level read and returns
  # them as a string. Do not mix with other methods that read from
  # ios or you may get unpredictable results. Raises SystemCallError
  # on error and EOFError at end of file.
  #
  #  f = File.new("testfile")
  #  f.sysread(16)   #=> "This is line one"
  #
  #  @todo  Improve reading into provided buffer.
  #
  def sysread(number_of_bytes, buffer=undefined)
    flush
    raise IOError unless @ibuffer.empty?

    str = read_primitive number_of_bytes
    raise EOFError if str.nil?

    unless undefined.equal? buffer
      StringValue(buffer).replace str
    end

    str
  end

  ##
  # Seeks to a given offset in the stream according to the value
  # of whence (see IO#seek for values of whence). Returns the new offset into the file.
  #
  #  f = File.new("testfile")
  #  f.sysseek(-13, IO::SEEK_END)   #=> 53
  #  f.sysread(10)                  #=> "And so on."
  def sysseek(amount, whence=SEEK_SET)
    ensure_open
    if @ibuffer.write_synced?
      raise IOError unless @ibuffer.empty?
    else
      warn 'sysseek for buffered IO'
    end

    amount = Integer(amount)

    prim_seek amount, whence
  end

  def to_io
    self
  end

  ##
  # Returns true if ios is associated with a terminal device (tty), false otherwise.
  #
  #  File.new("testfile").isatty   #=> false
  #  File.new("/dev/tty").isatty   #=> true
  def tty?
    ensure_open
    FFI::Platform::POSIX.isatty(@descriptor) == 1
  end

  alias_method :isatty, :tty?

  alias_method :prim_write, :write
  alias_method :prim_close, :close

  def write(data)
    data = String data
    return 0 if data.length == 0

    ensure_open_and_writable

    if @sync
      prim_write(data)
    else
      @ibuffer.unseek! self
      bytes_to_write = data.size

      while bytes_to_write > 0
        bytes_to_write -= @ibuffer.unshift(data, data.size - bytes_to_write)
        @ibuffer.empty_to self if @ibuffer.full? or sync
      end
    end

    data.size
  end

  def syswrite(data)
    data = String data
    return 0 if data.length == 0

    ensure_open_and_writable
    @ibuffer.unseek!(self) unless @sync

    prim_write(data)
  end

  def write_nonblock(data)
    ensure_open_and_writable

    data = String data
    return 0 if data.length == 0

    @ibuffer.unseek!(self) unless @sync

    raw_write(data)
  end

  def close
    begin
      flush
    ensure
      prim_close
    end

    if @pid and @pid != 0
      begin
        Process.wait @pid
      rescue Errno::ECHILD
        # If the child already exited
      end
      @pid = nil
    end

    return nil
  end

end

##
# Implements the pipe returned by IO::pipe.

class IO::BidirectionalPipe < IO

  def set_pipe_info(write)
    @write = write
    @sync = true
  end

  ##
  # Closes ios and flushes any pending writes to the
  # operating system. The stream is unavailable for
  # any further data operations; an IOError is raised
  # if such an attempt is made. I/O streams are
  # automatically closed when they are claimed by
  # the garbage collector.
  #
  # If ios is opened by IO.popen, close sets $?.
  def close
    @write.close unless @write.closed?

    super unless closed?

    nil
  end

  def closed?
    super and @write.closed?
  end

  def close_read
    raise IOError, 'closed stream' if closed?

    close
  end

  def close_write
    raise IOError, 'closed stream' if @write.closed?

    @write.close
  end

  # Expand these out rather than using some metaprogramming because it's a fixed
  # set and it's faster to have them as normal methods because then InlineCaches
  # work right.
  #
  def <<(obj)
    @write << obj
  end

  def print(*args)
    @write.print(*args)
  end

  def printf(fmt, *args)
    @write.printf(fmt, *args)
  end

  def putc(obj)
    @write.putc(obj)
  end

  def puts(*args)
    @write.puts(*args)
  end

  def syswrite(data)
    @write.syswrite(data)
  end

  def write(data)
    @write.write(data)
  end

  def write_nonblock(data)
    @write.write_nonblock(data)
  end

end

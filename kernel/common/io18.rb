# -*- encoding: us-ascii -*-

class IO

  class InternalBuffer
    alias_method :getchar, :getbyte
  end

  def self.for_fd(fd, mode = nil)
    new fd, mode
  end

  def self.foreach(name, sep_string=$/)
    return to_enum(:foreach, name, sep_string) unless block_given?

    name = StringValue(name)

    if name[0] == ?|
      io = IO.popen(name[1..-1], "r")
      return nil unless io
    else
      io = File.open(name, 'r')
    end

    if sep_string.nil?
      sep = sep_string
    else
      sep = StringValue(sep_string)
    end

    saved_line = $_

    begin
      while line = io.gets(sep)
        yield line
      end
    ensure
      $_ = saved_line
      io.close
    end

    return nil
  end

  def self.readlines(name, sep_string=$/)
    name = StringValue name

    if name[0] == ?|
      io = IO.popen(name[1..-1], "r")
      return nil unless io
    else
      io = File.open(name, 'r')
    end

    begin
      io.readlines sep_string
    ensure
      io.close
    end
  end

  ##
  # Opens the file, optionally seeks to the given offset,
  # then returns length bytes (defaulting to the rest of
  # the file). read ensures the file is closed before returning.
  #
  #  IO.read("testfile")           #=> "This is line one\nThis is line two\nThis is line three\nAnd so on...\n"
  #  IO.read("testfile", 20)       #=> "This is line one\nThi"
  #  IO.read("testfile", 20, 10)   #=> "ne one\nThis is line "
  def self.read(name, length=undefined, offset=0)
    name = Rubinius::Type.coerce_to_path name
    length ||= undefined
    offset ||= 0

    offset = Rubinius::Type.coerce_to(offset, Fixnum, :to_int)

    if offset < 0
      raise Errno::EINVAL, "offset must not be negative"
    end

    unless undefined.equal?(length)
      length = Rubinius::Type.coerce_to(length, Fixnum, :to_int)

      if length < 0
        raise ArgumentError, "length must not be negative"
      end
    end

    # Detect pipe mode
    if name[0] == ?|
      io = IO.popen(name[1..-1], "r")
      return nil unless io # child process
    else
      io = File.new(name)
    end

    str = nil
    begin
      io.seek(offset) unless offset == 0

      if undefined.equal?(length)
        str = io.read
      else
        str = io.read length
      end
    ensure
      io.close
    end

    return str
  end

  #
  # Create a new IO associated with the given fd.
  #
  def initialize(fd, mode=nil)
    if block_given?
      warn 'IO::new() does not take block; use IO::open() instead'
    end

    IO.setup self, Rubinius::Type.coerce_to(fd, Integer, :to_int), mode
  end

  private :initialize

  alias_method :getbyte, :getc

  def read(length=nil, buffer=nil)
    ensure_open_and_readable
    buffer = StringValue(buffer) if buffer

    unless length
      str = read_all
      return str unless buffer

      buffer.replace str
      return buffer
    end

    if @ibuffer.exhausted?
      buffer.replace buffer[0, length] if buffer and length < buffer.size
      return nil
    end

    str = ""
    needed = length
    while needed > 0 and not @ibuffer.exhausted?
      available = @ibuffer.fill_from self

      count = available > needed ? needed : available
      str << @ibuffer.shift(count)
      str = nil if str.empty?

      needed -= count
    end

    return str unless buffer

    if str
      buffer.replace str
      buffer
    else
      buffer.replace ''
      nil
    end
  end

  ##
  # Chains together buckets of input from the buffer until
  # locating +sep+. If +sep+ is +nil+, returns +read_all+.
  # If +sep+ is +""+, reads until +"\n\n"+. Otherwise, reads
  # until +sep+. This is the behavior of +#each+ and +#gets+.
  # Also, sets +$.+ for each line read. Returns the line read
  # or +nil+ if <tt>#eof?</tt> is true.
  #--
  # This implementation is slightly longer to reduce several
  # method calls. The common case is that the buffer has
  # enough data to just find the sep and return. Instead of
  # returning nil right of if buffer.exhausted?, allow the
  # nil to fall through. Also, don't create an empty string
  # just to append a single line to it.
  #++
  def read_to_separator(sep)
    return if @ibuffer.exhausted?
    return read_all unless sep

    sep = StringValue sep if sep

    if sep.empty?
      sep = "\n\n"
      skip = ?\n
    else
      skip = nil
    end

    line = nil
    until @ibuffer.exhausted?
      @ibuffer.fill_from self, skip

      if count = @ibuffer.find(sep)
        str = @ibuffer.shift(count)
      else
        str = @ibuffer.shift
      end

      if line
        line << str
      else
        line = str
      end

      break if count
    end

    @ibuffer.discard skip if skip

    line unless line.empty?
  end

  private :read_to_separator

  ##
  # Executes the block for every line in ios, where
  # lines are separated by sep_string. ios must be
  # opened for reading or an IOError will be raised.
  #
  #  f = File.new("testfile")
  #  f.each { |line| puts "#{f.lineno}: #{line}" }
  # produces:
  #
  #  1: This is line one
  #  2: This is line two
  #  3: This is line three
  #  4: And so on...
  def each(sep=$/)
    return to_enum(:each, sep) unless block_given?

    ensure_open_and_readable

    sep = sep.to_str if sep
    while line = read_to_separator(sep)
      @lineno += 1
      $. = @lineno
      yield line
    end

    self
  end

  alias_method :each_line, :each

  def each_char
    return to_enum(:each_char) unless block_given?

    ensure_open_and_readable
    if Rubinius.kcode == :UTF8
      # TODO zoinks. This is the slowest way possible to do this.
      # We'll have to rewrite it.
      lookup = 7.downto(4)
      while c = read(1) do
        n = c[0]
        leftmost_zero_bit = lookup.find { |i| n[i] == 0 }

        case leftmost_zero_bit
        when 7 # ASCII
          yield c
        when 6 # UTF 8 complementary characters
          next # Encoding error, ignore
        else
          more = read(6 - leftmost_zero_bit)
          break unless more
          yield c + more
        end
      end
    else
      while s = read(1)
        yield s
      end
    end

    self
  end

  alias_method :chars, :each_char

  ##
  # Reads the next ``line’’ from the I/O stream;
  # lines are separated by sep_string. A separator
  # of nil reads the entire contents, and a zero-length
  # separator reads the input a paragraph at a time (two
  # successive newlines in the input separate paragraphs).
  # The stream must be opened for reading or an IOError
  # will be raised. The line read in will be returned and
  # also assigned to $_. Returns nil if called at end of file.
  #
  #  File.new("testfile").gets   #=> "This is line one\n"
  #  $_                          #=> "This is line one\n"
  def gets(sep=$/)
    ensure_open_and_readable

    if line = read_to_separator(sep)
      line.taint
      @lineno += 1
      $. = @lineno
    end

    $_ = line
  end

  ##
  # Writes the given objects to ios as with IO#print.
  # Writes a record separator (typically a newline)
  # after any that do not already end with a newline
  # sequence. If called with an array argument, writes
  # each element on a new line. If called without arguments,
  # outputs a single record separator.
  #
  #  $stdout.puts("this", "is", "a", "test")
  # produces:
  #
  #  this
  #  is
  #  a
  #  test
  def puts(*args)
    if args.empty?
      write DEFAULT_RECORD_SEPARATOR
    else
      args.each do |arg|
        if arg.equal? nil
          str = "nil"
        elsif Thread.guarding? arg
          str = "[...]"
        elsif arg.kind_of?(Array)
          Thread.recursion_guard arg do
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

  def binmode
    ensure_open
    # HACK what to do?
    self
  end

  def self.pipe
    lhs = allocate
    rhs = allocate

    begin
      connect_pipe(lhs, rhs)
    rescue Errno::EMFILE
      GC.run(true)
      connect_pipe(lhs, rhs)
    end

    lhs.sync = true
    rhs.sync = true
    return [lhs, rhs]
  end

  ##
  # Runs the specified command string as a subprocess;
  # the subprocess‘s standard input and output will be
  # connected to the returned IO object. If cmd_string
  # starts with a ``-’’, then a new instance of Ruby is
  # started as the subprocess. The default mode for the
  # new file object is ``r’’, but mode may be set to any
  # of the modes listed in the description for class IO.
  #
  # If a block is given, Ruby will run the command as a
  # child connected to Ruby with a pipe. Ruby‘s end of
  # the pipe will be passed as a parameter to the block.
  # At the end of block, Ruby close the pipe and sets $?.
  # In this case IO::popen returns the value of the block.
  #
  # If a block is given with a cmd_string of ``-’’, the
  # block will be run in two separate processes: once in
  # the parent, and once in a child. The parent process
  # will be passed the pipe object as a parameter to the
  # block, the child version of the block will be passed
  # nil, and the child‘s standard in and standard out will
  # be connected to the parent through the pipe.
  # Not available on all platforms.
  #
  #  f = IO.popen("uname")
  #  p f.readlines
  #  puts "Parent is #{Process.pid}"
  #  IO.popen ("date") { |f| puts f.gets }
  #  IO.popen("-") { |f| $stderr.puts "#{Process.pid} is here, f is #{f}"}
  #  p $?
  # produces:
  #
  #  ["Linux\n"]
  #  Parent is 26166
  #  Wed Apr  9 08:53:52 CDT 2003
  #  26169 is here, f is
  #  26166 is here, f is #<IO:0x401b3d44>
  #  #<Process::Status: pid=26166,exited(0)>
  def self.popen(str, mode="r")
    mode = parse_mode mode

    readable = false
    writable = false

    if mode & IO::RDWR != 0
      readable = true
      writable = true
    elsif mode & IO::WRONLY != 0
      writable = true
    else # IO::RDONLY
      readable = true
    end

    pa_read, ch_write = pipe if readable
    ch_read, pa_write = pipe if writable

    pid = Process.fork

    # child
    if !pid
      if readable
        pa_read.close
        STDOUT.reopen ch_write
      end

      if writable
        pa_write.close
        STDIN.reopen ch_read
      end

      if str == "-"
        if block_given?
          yield nil
          exit! 0
        else
          return nil
        end
      else
        Process.perform_exec str, []
      end
    end

    ch_write.close if readable
    ch_read.close  if writable

    # We only need the Bidirectional pipe if we're reading and writing.
    # If we're only doing one, we can just return the IO object for
    # the proper half.
    #
    if readable and writable
      # Transmogrify pa_read into a BidirectionalPipe object,
      # and then tell it abou it's pid and pa_write

      Rubinius::Unsafe.set_class pa_read, IO::BidirectionalPipe

      pipe = pa_read
      pipe.set_pipe_info(pa_write)
    elsif readable
      pipe = pa_read
    elsif writable
      pipe = pa_write
    else
      raise ArgumentError, "IO is neither readable nor writable"
    end

    pipe.pid = pid

    return pipe unless block_given?

    begin
      yield pipe
    ensure
      pipe.close unless pipe.closed?
    end
  end

  ##
  # Return a string describing this IO object.
  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)}>"
  end

  def lines(*args)
    to_enum :each_line, *args
  end

  def ungetc(chr)
    ensure_open

    @ibuffer.put_back chr
    nil
  end
end

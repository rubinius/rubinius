class StringIO

  include Enumerable

  DEFAULT_RECORD_SEPARATOR = "\n" unless defined?(::DEFAULT_RECORD_SEPARATOR)

  def self.open(*args)
    io = new(*args)
    return io unless block_given?

    begin
      yield io
    ensure
      io.send(:finalize)
    end
  end

  attr_reader :string, :pos
  attr_accessor :lineno

  def initialize(string = "", mode = nil)
    @string = Rubinius::Type.coerce_to string, String, :to_str
    @pos = 0
    @lineno = 0

    if mode
      if mode.is_a?(Integer)
        mode_from_integer(mode)
      else
        mode = StringValue(mode)
        mode_from_string(mode)
      end
    else
      mode_from_string(@string.frozen? ? "r" : "r+")
    end

    self
  end

  def initialize_copy(from)
    from = Rubinius::Type.coerce_to(from, StringIO, :to_strio)

    taint if from.tainted?

    @string = from.instance_variable_get(:@string).dup
    @append = from.instance_variable_get(:@append)
    @readable = from.instance_variable_get(:@readable)
    @writable = from.instance_variable_get(:@writable)

    @pos = from.instance_variable_get(:@pos)
    @lineno = from.instance_variable_get(:@lineno)

    self
  end

  def each_byte
    return to_enum :each_byte unless block_given?
    raise IOError, "not opened for reading" unless @readable
    if @pos < @string.length
      @string.byteslice(@pos..-1).each_byte { |b| @pos += 1; yield b}
    end
    self
  end

  alias_method :bytes, :each_byte

  def each_char
    return to_enum :each_char unless block_given?
    if $KCODE == "UTF8"
      lookup = 7.downto(4)
      while c = read(1) do
        n = c[0].ord
        leftmost_zero_bit = lookup.find{|i| n[i].zero? }
        case leftmost_zero_bit
        when 7 # ASCII
          yield c
        when 6 # UTF 8 complementary characters
          next # Encoding error, ignore
        else
          more = read(6-leftmost_zero_bit)
          break unless more
          yield c+more
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

  def each(*args)
    return to_enum :each, *args unless block_given?
    raise IOError, "not opened for reading" unless @readable
    while line = getline(*args)
      yield line
    end
    self
  end
  alias_method :each_line, :each
  alias_method :lines, :each

  def <<(str)
    write(str)
    self
  end

  def binmode
    self
  end

  def write(str)
    raise IOError, "not opened for writing" unless @writable

    str = String(str)

    return 0 if str.empty?

    if @append || @pos == @string.length
      @string << str
      @pos = @string.length
    elsif @pos > @string.bytesize
      @string[@string.bytesize .. @pos] = "\000" * (@pos - @string.bytesize)
      @string << str
      @pos = @string.bytesize
    else
      @string[@pos, str.length] = str
      @pos += str.length
      @string.taint if str.tainted?
    end

    return str.length
  end
  alias_method :syswrite, :write

  def close
    raise IOError, "closed stream" if closed?
    @readable = @writable = nil
  end

  def closed?
    !@readable && !@writable
  end

  def close_read
    raise IOError, "closing non-duplex IO for reading" unless @readable
    @readable = nil
  end

  def closed_read?
    !@readable
  end

  def close_write
    raise IOError, "closing non-duplex IO for writing" unless @writable
    @writable = nil
  end

  def closed_write?
    !@writable
  end

  def eof?
    @pos >= @string.bytesize
  end
  alias_method :eof, :eof?

  def fcntl
    raise NotImplementedError, "StringIO#fcntl is not implemented"
  end

  def fileno
    nil
  end

  def flush
    self
  end

  def fsync
    0
  end

  def getc
    raise IOError, "not opened for reading" unless @readable
    char = @string[@pos]
    @pos += 1 unless eof?
    char
  end

  def getbyte
    char = getc
    char && char.ord
  end

  def gets(*args)
    $_ = getline(*args)
  end

  def isatty
    false
  end
  alias_method :tty?, :isatty

  def length
    @string.length
  end
  alias_method :size, :length

  def pid
    nil
  end

  def pos=(pos)
    raise Errno::EINVAL if pos < 0
    @pos = pos
  end

  def print(*args)
    raise IOError, "not opened for writing" unless @writable
    args << $_ if args.empty?
    write((args << $\).flatten.join)
    nil
  end

  def printf(*args)
    raise IOError, "not opened for writing" unless @writable

    if args.size > 1
      write(args.shift % args)
    else
      write(args.first)
    end

    nil
  end

  def putc(obj)
    raise IOError, "not opened for writing" unless @writable

    if obj.is_a?(String)
      char = obj[0]
    else
      char = Rubinius::Type.coerce_to obj, Integer, :to_int
    end

    if @append || @pos == @string.length
      @string << char
      @pos = @string.length
    elsif @pos > @string.length
      @string[@string.length .. @pos] = "\000" * (@pos - @string.length)
      @string << char
      @pos = @string.length
    else
      @string[@pos] = char
      @pos += 1
    end

    obj
  end

  def puts(*args)
    if args.empty?
      write(DEFAULT_RECORD_SEPARATOR)
    else
      args.each do |arg|
        if arg.nil?
          line = "nil"
        elsif Thread.guarding? arg
          line = "[...]"
        else
          begin
            arg = Rubinius::Type.coerce_to(arg, Array, :to_ary)
            Thread.recursion_guard arg do
              arg.each { |a| puts a }
            end
            next
          rescue
            line = arg.to_s
          end
        end

        write(line)
        write(DEFAULT_RECORD_SEPARATOR) unless line[-1] == ?\n
      end
    end

    nil
  end

  def read(length = nil, buffer = "")
    raise IOError, "not opened for reading" unless @readable

    buffer = StringValue(buffer)

    if length
      return nil if eof?
      length = Rubinius::Type.coerce_to length, Integer, :to_int
      raise ArgumentError if length < 0
      buffer.replace @string.byteslice(@pos, length)
      buffer.force_encoding Encoding::ASCII_8BIT
      @pos += buffer.length
    else
      return "" if eof?
      buffer.replace @string.byteslice(@pos..-1)
      @pos = @string.bytesize
    end

    return buffer
  end

  def readchar
    raise IO::EOFError, "end of file reached" if eof?
    getc
  end

  def readbyte
    readchar.getbyte(0)
  end

  def readline(*args)
    raise IO::EOFError, "end of file reached" if eof?
    $_ = getline(*args)
  end

  def readlines(*args)
    raise IOError, "not opened for reading" unless @readable
    ary = []
    while line = getline(*args)
      ary << line
    end
    ary
  end

  def reopen(string = nil, mode = nil)
    if string
      if !string.is_a?(String) and !mode
        string = Rubinius::Type.coerce_to(string, StringIO, :to_strio)
        taint if string.tainted?
        @string = string.string
      else
        @string = StringValue(string)

        if mode
          if mode.is_a?(Integer)
            mode_from_integer(mode)
          else
            mode = StringValue(mode)
            mode_from_string(mode)
          end
        else
          mode_from_string("r+")
        end
      end
    else
      mode_from_string("r+")
    end

    @pos = 0
    @lineno = 0

    self
  end

  def rewind
    @pos = 0
    @lineno = 0
  end

  def seek(to, whence = IO::SEEK_SET)
    raise IOError, "closed stream" if self.closed?
    to = Rubinius::Type.coerce_to to, Integer, :to_int

    case whence
    when IO::SEEK_CUR
      to += @pos
    when IO::SEEK_END
      to += @string.bytesize
    when IO::SEEK_SET, nil
    else
      raise Errno::EINVAL, "invalid whence"
    end

    raise Errno::EINVAL if to < 0

    @pos = to

    return 0
  end

  def string=(string)
    @string = StringValue(string)
    @pos = 0
    @lineno = 0
  end

  def sync
    true
  end

  def sync=(val)
    val
  end

  def sysread(length = nil, buffer = "")
    str = read(length, buffer)
    if str.nil?
      buffer.clear
      raise IO::EOFError, "end of file reached"
    end
    str
  end

  alias_method :readpartial, :sysread
  alias_method :read_nonblock, :sysread

  def tell
    @pos
  end

  def truncate(length)
    raise IOError, "not opened for writing" unless @writable
    len = Rubinius::Type.coerce_to length, Integer, :to_int
    raise Errno::EINVAL, "negative length" if len < 0
    if len < @string.bytesize
      @string[len .. @string.bytesize] = ""
    else
      @string << "\000" * (len - @string.bytesize)
    end
    return length
  end

  def ungetc(char)
    raise IOError, "not opened for reading" unless @readable
    if char.kind_of? Integer
      char = Rubinius::Type.coerce_to char, String, :chr
    else
      char = Rubinius::Type.coerce_to char, String, :to_str
    end

    if @pos > @string.bytesize
      @string[@string.bytesize .. @pos] = "\000" * (@pos - @string.bytesize)
      @pos -= 1
      @string[@pos] = char
    elsif @pos > 0
      @pos -= 1
      @string[@pos] = char
    end

    nil
  end

  protected
    def finalize
      close
      @string = nil
      self
    end

    def mode_from_string(mode)
      @readable = @writable = @append = truncate = false

      case mode
      when "r", "rb"
        @readable = true
      when "r+", "rb+"
        @readable = true
        @writable = true
      when "w", "wb"
        truncate = true
        @writable = true
      when "w+", "wb+"
        truncate = true
        @readable = true
        @writable = true
      when "a", "ab"
        @writable = true
        @append   = true
      when "a+", "ab+"
        @readable = true
        @writable = true
        @append   = true
      end
      raise Errno::EACCES, "Permission denied" if @writable && @string.frozen?
      @string.replace("") if truncate
    end

    def mode_from_integer(mode)
      @readable = @writable = @append = false

      case mode & (IO::RDONLY | IO::WRONLY | IO::RDWR)
      when IO::RDONLY
        @readable = true
        @writable = false
      when IO::WRONLY
        raise Errno::EACCES, "Permission denied" if @string.frozen?
        @readable = false
        @writable = true
      when IO::RDWR
        raise Errno::EACCES, "Permission denied" if @string.frozen?
        @readable = true
        @writable = true
      end

      @append = true if (mode & IO::APPEND) != 0
      @string.replace("") if (mode & IO::TRUNC) != 0
    end

    def getline(*args)
      raise IOError unless @readable

      sep = nil
      limit = nil

      case args.size
      when 0
        sep = $/
      when 1
        if args[0]
          if limit = Rubinius::Type.check_convert_type(args[0], Integer, :to_int)
            return '' if limit == 0
          else
            sep = Rubinius::Type.coerce_to(args[0], String, :to_str)
          end
        end
      when 2
        sep = Rubinius::Type.check_convert_type(args[0], String, :to_str)
        limit = Rubinius::Type.coerce_to(args[1], Integer, :to_int)
      end

      sep = StringValue(sep) unless sep.nil?

      return nil if eof?

      if sep.nil?
        if limit
          line = @string.byteslice(@pos...@pos + limit)
        else
          line = @string.byteslice(@pos..-1)
        end
        @pos += line.bytesize
      elsif sep.empty?
        if stop = @string.index("\n\n", @pos)
          stop += 2
          line = @string.byteslice(@pos...stop)
          while @string[stop] == ?\n
            stop += 1
          end
          @pos = stop
        else
          line = @string.byteslice(@pos .. -1)
          @pos = @string.bytesize
        end
      else
        if stop = @string.index(sep, @pos)
          if limit && stop - @pos >= limit
            stop = @pos + limit
          else
            stop += sep.length
          end
          line = @string.byteslice(@pos...stop)
          @pos = stop
        else
          if limit
            line = @string.byteslice(@pos...@pos + limit)
          else
            line = @string.byteslice(@pos..-1)
          end
          @pos += line.bytesize
        end
      end

      @lineno += 1

      return line
    end
end

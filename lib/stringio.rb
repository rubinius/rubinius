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

  def initialize(string = "", mode = Undefined)
    @string = Type.coerce_to string, String, :to_str
    @pos = 0
    @lineno = 0

    unless mode == Undefined
      if mode.is_a?(Integer)
        mode_from_integer(mode)
      else
        mode = StringValue(mode)
        mode_from_string(mode)
      end
    else
      mode_from_string("r+")
    end

    self
  end

  def initialize_copy(from)
    from = Type.coerce_to(from, StringIO, :to_strio)

    self.taint if from.tainted?

    @string = from.instance_variable_get(:@string).dup
    @append = from.instance_variable_get(:@append)
    @readable = from.instance_variable_get(:@readable)
    @writable = from.instance_variable_get(:@writable)

    @pos = from.instance_variable_get(:@pos)
    @lineno = from.instance_variable_get(:@lineno)

    self
  end

  def <<(str)
    self.write(str)
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
    elsif @pos > @string.size
      @string[@string.size .. @pos] = "\000" * (@pos - @string.size)
      @string << str
      @pos = @string.size
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

  def each_byte
    raise IOError, "not opened for reading" unless @readable
    if @pos < @string.length
      @string[@pos..-1].each_byte { |b| @pos += 1; yield b}
    end
    nil
  end

  def each(sep = $/)
    raise IOError, "not opened for reading" unless @readable
    sep = StringValue(sep) unless sep.nil?
    while line = self.getline(sep)
      yield line
    end
    self
  end
  alias_method :each_line, :each

  def eof?
    @pos >= @string.size
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
    @pos += 1 unless self.eof?
    char
  end

  def gets(sep = $/)
    $_ = self.getline(sep)
  end

  def isatty
    false
  end
  alias_method :tty?, :isatty

  def length
    @string.length
  end
  alias_method :size, :length

  def path
    nil
  end

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
    args.map! { |x| x.nil? ? "nil" : x }
    self.write((args << $\).flatten.join)
    nil
  end

  def printf(*args)
    raise IOError, "not opened for writing" unless @writable

    if args.size > 1
      self.write(args.shift % args)
    else
      self.write(args.first)
    end

    nil
  end

  def putc(obj)
    raise IOError, "not opened for writing" unless @writable

    if obj.is_a?(String)
      char = obj[0]
    else
      char = Type.coerce_to obj, Integer, :to_int
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
      self.write(DEFAULT_RECORD_SEPARATOR)
    else
      args.each do |arg|
        if arg.nil?
          line = "nil"
        elsif RecursionGuard.inspecting?(arg)
          line = "[...]"
        else
          begin
            arg = Type.coerce_to(arg, Array, :to_ary)
            RecursionGuard.inspect(arg) do
              arg.each { |a| self.puts a }
            end
            next
          rescue
            line = arg.to_s
          end
        end

        self.write(line)
        self.write(DEFAULT_RECORD_SEPARATOR) if !line.empty? && line[-1] != ?\n
      end
    end

    nil
  end

  def read(length = Undefined, buffer = "")
    raise IOError, "not opened for reading" unless @readable

    buffer = StringValue(buffer)

    if length == Undefined or length.nil?
      return "" if self.eof?
      buffer.replace(@string[@pos..-1])
      @pos = @string.size
    else
      return nil if self.eof?
      length = Type.coerce_to length, Integer, :to_int
      raise ArgumentError if length < 0
      buffer.replace(@string[@pos, length])
      @pos += buffer.length
    end

    return buffer
  end

  def readchar
    raise IO::EOFError, "end of file reached" if self.eof?
    getc
  end

  def readline(sep = $/)
    raise IO::EOFError, "end of file reached" if self.eof?
    $_ = self.getline(sep)
  end

  def readlines(sep = $/)
    raise IOError, "not opened for reading" unless @readable
    ary = []
    while line = self.getline(sep)
      ary << line
    end
    ary
  end

  def reopen(string = Undefined, mode = Undefined)
    unless string == Undefined
      if !string.is_a?(String) && mode == Undefined
        string = Type.coerce_to(string, StringIO, :to_strio)
        self.taint if string.tainted?
        @string = string.string
      else
        @string = StringValue(string)

        unless mode == Undefined
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
    #raise IOError if self.closed?
    to = Type.coerce_to to, Integer, :to_int

    case whence
    when IO::SEEK_CUR
      to += @pos
    when IO::SEEK_END
      to += @string.size
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

  def sysread(length = Undefined, buffer = "")
    str = read(length, buffer)
    raise IO::EOFError, "end of file reached" if str.nil?
    str
  end

  def tell
    @pos
  end

  def truncate(length)
    raise IOError, "not opened for writing" unless @writable
    len = Type.coerce_to length, Integer, :to_int
    raise Errno::EINVAL, "negative length" if len < 0
    if len < @string.size
      @string[len .. @string.size] = ""
    else
      @string << "\000" * (len - @string.size)
    end
    return length
  end

  def ungetc(char)
    raise IOError, "not opened for reading" unless @readable
    char = Type.coerce_to char, Integer, :to_int

    if @pos > @string.size
      @string[@string.size .. @pos] = "\000" * (@pos - @string.size)
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
      self.close
      @string = nil
      self
    end

    def mode_from_string(mode)
      @readable = @writable = @append = false

      case mode
      when "r", "rb"
        @readable = true
      when "r+", "rb+"
        @readable = true
        @writable = true
      when "w", "wb"
        @string.replace("")
        @writable = true
      when "w+", "wb+"
        @readable = true
        @writable = true
        @string.replace("")
      when "a", "ab"
        @writable = true
        @append   = true
      when "a+", "ab+"
        @readable = true
        @writable = true
        @append   = true
      end
    end

    def mode_from_integer(mode)
      @readable = @writable = @append = false

      case mode & (IO::RDONLY | IO::WRONLY | IO::RDWR)
      when IO::RDONLY
        @readable = true
        @writable = false
      when IO::WRONLY
        @readable = false
        @writable = true
      when IO::RDWR
        @readable = true
        @writable = true
      end

      @append = true if (mode & IO::APPEND) != 0
      @string.replace("") if (mode & IO::TRUNC) != 0
    end

    def getline(sep = $/)
      raise IOError unless @readable

      sep = StringValue(sep) unless sep.nil?

      return nil if self.eof?

      if sep.nil?
        line = @string[@pos .. -1]
        @pos = @string.size
      elsif sep.empty?
        if stop = @string.index("\n\n", @pos)
          stop += 2
          line = @string[@pos .. stop - 2]
          while @string[stop] == ?\n
            stop += 1
          end
          @pos = stop
        else
          line = @string[@pos .. -1]
          @pos = @string.size
        end
      else
        if stop = @string.index(sep, @pos)
          line = @string[@pos .. stop]
          @pos = stop + 1
        else
          line = @string[@pos .. -1]
          @pos = @string.size
        end
      end

      @lineno += 1

      return line
    end
end

class StringIO

  include Enumerable

  DEFAULT_RECORD_SEPARATOR = "\n" unless defined?(::DEFAULT_RECORD_SEPARATOR)

  # This is why we need undefined in Ruby
  Undefined = Object.new

  class Data
    attr_accessor :string, :pos, :lineno

    def initialize(string)
      @string = string
      @pos = @lineno = 0
    end
  end

  def self.open(*args)
    io = new(*args)
    return io unless block_given?

    begin
      yield io
    ensure
      io.close
      io.__data__.string = nil
      self
    end
  end

  attr_reader :__data__

  def initialize(string="", mode=nil)
    string = Rubinius::Type.coerce_to string, String, :to_str
    @__data__ = Data.new string

    if mode
      if mode.is_a?(Integer)
        mode_from_integer(mode)
      else
        mode = StringValue(mode)
        mode_from_string(mode)
      end
    else
      mode_from_string(string.frozen? ? "r" : "r+")
    end

    self
  end

  def initialize_copy(from)
    from = Rubinius::Type.coerce_to(from, StringIO, :to_strio)

    taint if from.tainted?

    @append = from.instance_variable_get(:@append)
    @readable = from.instance_variable_get(:@readable)
    @writable = from.instance_variable_get(:@writable)
    @__data__ = from.instance_variable_get(:@__data__)

    self
  end

  def check_readable
    raise IOError, "not opened for reading" unless @readable
  end

  private :check_readable

  def check_writable
    raise IOError, "not opened for writing" unless @writable
    raise IOError, "unable to modify data" if @__data__.string.frozen?
  end

  private :check_writable

  def each_byte
    return to_enum :each_byte unless block_given?
    check_readable

    d = @__data__
    string = d.string

    while d.pos < string.length
      byte = string.getbyte d.pos
      d.pos += 1
      yield byte
    end

    self
  end

  alias_method :bytes, :each_byte

  def each_char
    return to_enum :each_char unless block_given?
    if $KCODE == "UTF8"
      lookup = 7.downto(4)
      while c = read(1) do
        n = c[0]
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

  def each(sep = $/)
    return to_enum :each, sep unless block_given?
    check_readable

    while line = getline(sep)
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
    check_writable

    str = String(str)

    return 0 if str.empty?

    d = @__data__
    pos = d.pos
    string = d.string

    if @append || pos == string.length
      string << str
      d.pos = string.length
    elsif pos > string.bytesize
      string[string.bytesize..pos] = "\000" * (pos - string.bytesize)
      string << str
      d.pos = string.bytesize
    else
      string[pos, str.length] = str
      d.pos += str.length
      string.taint if str.tainted?
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
    check_readable
    @readable = nil
  end

  def closed_read?
    !@readable
  end

  def close_write
    check_writable
    @writable = nil
  end

  def closed_write?
    !@writable
  end

  def eof?
    d = @__data__
    d.pos >= d.string.bytesize
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
    check_readable
    d = @__data__

    char = d.string[d.pos]
    d.pos += 1 unless eof?
    char
  end

  def getbyte
    check_readable
    d = @__data__

    return nil if eof?

    byte = d.string.getbyte(d.pos)
    d.pos += 1
    byte
  end

  def gets(sep = $/)
    check_readable

    $_ = getline(sep)
  end

  def isatty
    false
  end
  alias_method :tty?, :isatty

  def length
    @string.length
  end
  alias_method :size, :length

  def lineno
    @__data__.lineno
  end

  def lineno=(line)
    @__data__.lineno = line
  end

  def path
    nil
  end

  def pid
    nil
  end

  def pos
    @__data__.pos
  end

  def pos=(pos)
    raise Errno::EINVAL if pos < 0
    @__data__.pos = pos
  end

  def print(*args)
    check_writable
    args << $_ if args.empty?
    args.map! { |x| x.nil? ? "nil" : x }
    write((args << $\).flatten.join)
    nil
  end

  def printf(*args)
    check_writable

    if args.size > 1
      write(args.shift % args)
    else
      write(args.first)
    end

    nil
  end

  def putc(obj)
    check_writable

    if obj.is_a?(String)
      char = obj[0]
    else
      char = Rubinius::Type.coerce_to obj, Integer, :to_int
    end

    d = @__data__
    pos = d.pos
    string = d.string

    if @append || pos == string.length
      string << char
      d.pos = string.length
    elsif pos > string.length
      string[string.length..pos] = "\000" * (pos - string.length)
      string << char
      d.pos = string.length
    else
      string[pos] = char
      d.pos += 1
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
    check_readable
    d = @__data__
    pos = d.pos
    string = d.string

    buffer = StringValue(buffer)

    if length
      return nil if eof?
      length = Rubinius::Type.coerce_to length, Integer, :to_int
      raise ArgumentError if length < 0
      buffer.replace(string[pos, length])
      d.pos += buffer.length
    else
      return "" if eof?
      buffer.replace(string[pos..-1])
      d.pos = string.size
    end

    return buffer
  end

  def readchar
    raise IO::EOFError, "end of file reached" if eof?
    getc
  end

  alias_method :readbyte, :readchar

  def readline(sep=$/)
    raise IO::EOFError, "end of file reached" if eof?
    check_readable

    $_ = getline(sep)
  end

  def readlines(sep=$/)
    check_readable

    ary = []
    while line = getline(sep)
      ary << line
    end

    ary
  end

  def reopen(string=nil, mode=Undefined)
    if string and not string.kind_of? String and mode.equal? Undefined
      stringio = Rubinius::Type.coerce_to(string, StringIO, :to_strio)

      taint if stringio.tainted?
      initialize_copy stringio
    else
      mode = nil if mode.equal? Undefined
      string = "" unless string

      initialize string, mode
    end

    self
  end

  def rewind
    d = @__data__
    d.pos = d.lineno = 0
  end

  def seek(to, whence = IO::SEEK_SET)
    raise IOError, "closed stream" if self.closed?
    to = Rubinius::Type.coerce_to to, Integer, :to_int

    case whence
    when IO::SEEK_CUR
      to += @__data__.pos
    when IO::SEEK_END
      to += @__data__.string.size
    when IO::SEEK_SET, nil
    else
      raise Errno::EINVAL, "invalid whence"
    end

    raise Errno::EINVAL if to < 0

    @__data__.pos = to

    return 0
  end

  def size
    @__data__.string.bytesize
  end
  alias_method :length, :size

  def string
    @__data__.string
  end

  def string=(string)
    d = @__data__
    d.string = StringValue(string)
    d.pos = 0
    d.lineno = 0
  end

  def sync
    true
  end

  def sync=(val)
    val
  end

  def sysread(length=nil, buffer="")
    str = read(length, buffer)
    raise IO::EOFError, "end of file reached" if str.nil?
    str
  end

  def tell
    @__data__.pos
  end

  def truncate(length)
    check_writable
    len = Rubinius::Type.coerce_to length, Integer, :to_int
    raise Errno::EINVAL, "negative length" if len < 0
    string = @__data__.string

    if len < string.size
      string[len..string.size] = ""
    else
      string << "\000" * (len - string.size)
    end
    return length
  end

  def ungetc(char)
    check_readable

    d = @__data__
    pos = d.pos
    string = d.string

    char = Rubinius::Type.coerce_to char, Integer, :to_int

    if pos > string.bytesize
      string[string.bytesize..pos] = "\000" * (pos - string.bytesize)
      d.pos -= 1
      string[d.pos] = char
    elsif pos > 0
      d.pos -= 1
      string[d.pos] = char
    end

    nil
  end

  def to_yaml_properties
    []
  end

  def yaml_initialize(type, val)
    @__data__ = Data.new("")
  end

  protected

  def mode_from_string(mode)
    @append = truncate = false

    if mode[0] == ?r
      @readable = true
      @writable = mode[-1] == ?+ ? true : false
    end

    if mode[0] == ?w
      @writable = truncate = true
      @readable = mode[-1] == ?+ ? true : false
    end

    if mode[0] == ?a
      @append = @writable = true
      @readable = mode[-1] == ?+ ? true : false
    end

    d = @__data__
    raise Errno::EACCES, "Permission denied" if @writable && d.string.frozen?
    d.string.replace("") if truncate
  end

  def mode_from_integer(mode)
    @readable = @writable = @append = false
    d = @__data__

    if mode == 0 or mode & IO::RDWR != 0
      @readable = true
    end

    if mode & (IO::WRONLY | IO::RDWR) != 0
      raise Errno::EACCES, "Permission denied" if d.string.frozen?
      @writable = true
    end

    @append = true if (mode & IO::APPEND) != 0
    d.string.replace("") if (mode & IO::TRUNC) != 0
  end

  def getline(sep = $/)
    sep = StringValue(sep) unless sep.nil?

    return nil if eof?

    d = @__data__
    pos = d.pos
    string = d.string

    if sep.nil?
      line = string[pos..-1]
      d.pos = string.size
    elsif sep.empty?
      if stop = string.index("\n\n", pos)
        stop += 2
        line = string[pos...stop]
        while string[stop] == ?\n
          stop += 1
        end
        d.pos = stop
      else
        line = string[pos..-1]
        d.pos = string.size
      end
    else
      if stop = string.index(sep, pos)
        stop += sep.length
        line = string[pos...stop]
        d.pos = stop
      else
        line = string[pos..-1]
        d.pos = string.size
      end
    end

    d.lineno += 1

    return line
  end
end

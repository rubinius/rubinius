class StringIO
  DEFAULT_RECORD_SEPARATOR = "\n" unless defined?(::DEFAULT_RECORD_SEPARATOR)

  def self.open(string="", mode=nil)
    obj = new(string, mode)
    return obj unless block_given?

    begin
      yield obj
    ensure
      obj.finalize
    end
  end

  def initialize(string="", mode=nil)
    @writable = false
    @readable = false
    @append = false

    if not string.kind_of?(String) then
      if string.respond_to?(:to_str) then
        string = string.to_str
      else
        raise TypeError, "can't convert #{string.class} into String"
      end
    end

    @string = string
    @mode = mode || "r+"
    @mode.gsub!(/b/,'') # Ignore binary mode flags

    if @mode.include?("+")
      @readable = true
      @writable = true
    elsif @mode == "r"
      @readable = true
    elsif @mode == "w"
      @writable = true
    elsif @mode == "a"
      @readable = true
      @writable = true
      @append = true
    end

    @pos = 0
    @eof = false
    @lineno = 0
  end

  def finalize
    @string = nil
    @readable = @writable = false
  end

  attr_reader :string, :mode

  def string=(str)
    @string = str
    @mode = "r+"
    @readable = true
    @writable = true
    @pos = 0
    @lineno = 0
    return str
  end

  def close
    @readable = false
    @writable = false
    return nil
  end

  def close_read
    @readable = false
  end

  def close_write
    @writable = false
  end

  def closed?
    @readable.equal?(false) and @writable.equal?(false)
  end

  def closed_read?
    @readable.equal?(false)
  end

  def closed_write?
    @writable.equal?(false)
  end

  def eof
    @eof or @pos >= @string.size
  end

  alias_method :eof?, :eof

  attr_accessor :lineno, :pos
  alias_method :tell, :pos

  def binmode
    self
  end

  def fcntl
    raise NotImplementedError, "Unable to run fcntl on a StringIO"
  end

  def flush
    self
  end

  def fsync
    0
  end

  def reopen(obj, mode=nil)
    if obj.kind_of? StringIO
      @string = obj.string
      @lineno = obj.lineno
      @pos =    obj.pos
      @readable = !obj.closed_read?
      @writable = !obj.closed_write?
      @mode =   obj.mode
    else
      initialize(obj, mode)
      if @mode =~ /^[wa]/
        @string.replace '' unless @append
      end
    end

    return self
  end

  def pos=(where)
    i = where.to_i
    if i < 0
      raise Errno::EINVAL, "Invalid position '#{i}'"
    end

    @pos = i
    if @pos >= @string.size
      @string << ("\0" * (@pos - @string.size))
    end
  end

  def rewind
    @pos = 0
    @lineno = 0
    @eof = false
  end

  def seek(amount, whence=nil)
    i = amount.to_i


    # TODO: prolly should be a platform specific check long/long long/whatever
    raise RangeError, "bignum too big" if Bignum === i

    if whence.nil? or whence == 0
      # nothing
    elsif whence == IO::SEEK_CUR
      i += @pos
    elsif whence == IO::SEEK_END
      i = @string.size + i
    else
      raise ArgumentError, "invalid whence #{whence}"
    end

    if i < 0
      raise ArgumentError, "invalid offset #{i}"
    end

    @pos = i

    if @pos >= @string.size - 1
      @eof = true
    else
      @eof = false
    end

    return 0
  end

  def sync
    true
  end

  def sync=(val)
    val
  end

  def each_byte
    raise IOError, "not opened for reading" unless @readable

    while @pos < @string.size
      yield @string[@pos]
      @pos += 1
    end

    nil
  end

  def getc
    if @pos >= @string.size
      @eof = true
      return nil
    end

    chr = @string[@pos]
    @pos += 1
    return chr
  end

  def readchar
    out = getc
    raise EOFError, "at end of buffer" if out.nil?
    return out
  end

  def ungetc(chr)
    unless chr.kind_of? Integer
      raise TypeError, "only accepts character code"
    end

    return nil if @pos == 0

    @eof = false
    @pos -= 1
    @string[@pos] = chr
    return nil
  end

  def gets(sep=$/)
    raise IOError, "not opened for reading" unless @readable
    return nil if @eof
    re = Regexp.new("^(.*?#{sep})", Regexp::MULTILINE)
    rest = @string[@pos..-1]
    if m = re.match(rest)
      @lineno += 1
      output = m[1]
      @pos += output.size
      return output
    else
      @eof = true
      return nil if rest.empty?
      return rest
    end
  end

  def readline
    out = gets
    raise EOFError, "at end of buffer" if out.nil?
    return out
  end

  def each(sep=$/)
    while line = gets(sep)
      yield line
    end
    return self
  end

  alias_method :each_line, :each

  def readlines(sep=$/)
    return [read] if sep.nil?

    out = []
    while line = gets(sep)
      out << line
    end

    return out
  end

  def write(str)
    raise IOError, "not opened for writing" unless @writable

    str = str.to_s unless str.kind_of? String

    len = str.size

    return 0 if len == 0

    if @append
      @pos = @string.size
    end

    if @pos == @string.size - 1
      @string << str
    else
      @string[@pos, len] = str
    end

    @pos += len

    return len
  end
  alias_method :syswrite, :write

  def <<(str)
    write str
    return self
  end

  def print(*args)
    i = 0
    args.each do |arg|
      if $\ and i > 0
        write $\
      end

      if arg.nil?
        write "nil"
      else
        write arg
      end
    end

    write $\ if $\

    return nil
  end

  def printf(str, *args)
    write(str % args)

    return nil
  end

  def putc(chr)
    if chr.kind_of? String
      write chr[0..0]
      return chr
    end

    @pos = @string.size if @append

    if @pos >= @string.size - 1 then
      @string << chr.chr
      @pos += 1
    else
      @string[@pos] = chr
    end

    return chr
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

  def read(length=nil, buffer=nil)
    raise IOError, "not opened for reading" unless @readable
    if @eof
      return nil if length
      return ""
    end

    if buffer and !buffer.kind_of? String
      raise TypeError, "buffer must be a String"
    end

    length = @string.size - @pos unless length

    data = @string[@pos, length]

    @pos += length
    if @pos >= @string.size - 1
      @eof = true
    end

    if buffer
      buffer.replace(data)
      return buffer
    end

    return data
  end

  def sysread(length=nil, buffer=nil)
    raise EOFError if @eof
    out = read(length, buffer)
    return out
  end

  def isatty
    false
  end

  alias_method :tty?, :isatty

  def pid
    nil
  end

  def fileno
    nil
  end

  def path
    nil
  end

  def size
    raise IOError, "not opened" unless @string
    return @string.size
  end

  alias_method :length, :size

  def truncate(count)
    @string[count, @string.size - count] = ""
    return count
  end
end

class IO
  attr_accessor :external
  attr_accessor :internal

  def self.binread(file, *arg)
    unless arg.size < 3
      raise ArgumentError, "wrong number of arguments (#{1+arg.size} for 1..3)"
    end

    File.open(file,"rb") do |f|
      f.read(*arg)
    end
  end

  def self.for_fd(fd, mode=undefined, options=undefined)
    new fd, mode, options
  end

  def self.read(name, length_or_options=undefined, offset=0, options=nil)
    offset = 0 if offset.nil?
    name = Rubinius::Type.coerce_to_path name
    mode = "r"

    if length_or_options.equal? undefined
      length = undefined
    elsif Rubinius::Type.object_kind_of? length_or_options, Hash
      length = undefined
      offset = 0
      options = length_or_options
    elsif length_or_options
      offset = Rubinius::Type.coerce_to(offset || 0, Fixnum, :to_int)
      raise Errno::EINVAL, "offset must not be negative" if offset < 0

      length = Rubinius::Type.coerce_to(length_or_options, Fixnum, :to_int)
      raise ArgumentError, "length must not be negative" if length < 0
    else
      length = undefined
    end

    if options
      mode = options[:mode] || "r"
    end

    # Detect pipe mode
    if name[0] == ?|
      io = IO.popen(name[1..-1], "r")
      return nil unless io # child process
    else
      io = File.new(name, mode)
    end

    str = nil
    begin
      io.seek(offset) unless offset == 0

      if length.equal?(undefined)
        str = io.read
      else
        str = io.read length
      end
    ensure
      io.close
    end

    return str
  end

  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, IO, :to_io
  end

  class StreamCopier
    def initialize(from, to, length, offset)
      @length = length
      @offset = offset

      @from_io, @from = to_io(from, "rb")
      @to_io, @to = to_io(to, "wb")

      @method = read_method @from
    end

    def to_io(obj, mode)
      if obj.kind_of? IO
        flag = true
        io = obj
      else
        flag = false

        if obj.kind_of? String
          io = File.open obj, mode
        elsif obj.respond_to? :to_path
          path = Rubinius::Type.coerce_to obj, String, :to_path
          io = File.open path, mode
        else
          io = obj
        end
      end

      return flag, io
    end

    def read_method(obj)
      if obj.respond_to? :readpartial
        :readpartial
      else
        :read
      end
    end

    def run
      @from.ensure_open_and_readable
      @to.ensure_open_and_writable

      saved_pos = @from.pos if @from_io

      @from.seek @offset, IO::SEEK_CUR if @offset

      size = rest = @length ? @length : 16384
      bytes = 0

      begin
        while data = @from.send(@method, size, "")
          @to.write data
          bytes += data.size
          rest -= data.size

          break unless rest > 0
        end
      rescue EOFError
        # done reading
      end

      @to.flush
      return bytes
    ensure
      if @from_io
        @from.pos = saved_pos if @offset
      else
        @from.close
      end

      @to.close unless @to_io
    end
  end

  def self.copy_stream(from, to, max_length=nil, offset=nil)
    StreamCopier.new(from, to, max_length, offset).run
  end

  #
  # Create a new IO associated with the given fd.
  #
  def initialize(fd, mode=undefined, options=undefined)
    if mode.equal? undefined
      mode = nil
    else
      if mode.kind_of? Hash
        options = mode
        mode = options[:mode]
      end
    end

    mode, external, internal = mode.split(":") if mode.kind_of? String

    if block_given?
      warn 'IO::new() does not take block; use IO::open() instead'
    end

    IO.setup self, Rubinius::Type.coerce_to(fd, Integer, :to_int), mode

    if external or internal
      set_encoding external, internal
    elsif !options.equal? undefined
      external = options[:external_encoding]
      internal = options[:internal_encoding]

      if external or internal
        set_encoding external, internal
      elsif encoding = options[:encoding]
        set_encoding encoding
      end
    end
  end

  private :initialize

  # Argument matrix for IO#gets and IO#each:
  #
  #  separator / limit | nil | >= 0 | < 0
  # ===================+=====+======+=====
  #  String (nonempty) |  A  |  B   |  C
  #                    +-----+------+-----
  #  ""                |  D  |  E   |  F
  #                    +-----+------+-----
  #  nil               |  G  |  H   |  I
  #

  class EachReader
    def initialize(io, buffer, separator, limit)
      @io = io
      @buffer = buffer
      @separator = separator
      @limit = limit
      @skip = nil
    end

    def each(&block)
      if @separator
        if @separator.empty?
          @separator = "\n\n"
          @skip = 10
        end

        if @limit
          read_to_separator_with_limit(&block)
        else
          read_to_separator(&block)
        end
      else
        if @limit
          read_to_limit(&block)
        else
          read_all(&block)
        end
      end
    end

    # method A, D
    def read_to_separator
      str = ""

      until @buffer.exhausted?
        available = @buffer.fill_from @io, @skip
        break unless available > 0

        if count = @buffer.find(@separator)
          str << @buffer.shift(count)

          str.taint
          $. = @io.increment_lineno
          @buffer.discard @skip if @skip

          yield str

          str = ""
        else
          str << @buffer.shift
        end
      end

      str << @buffer.shift
      unless str.empty?
        str.taint
        $. = @io.increment_lineno
        yield str
      end
    end

    # method B, E
    def read_to_separator_with_limit
      str = ""

      #TODO: implement ignoring encoding with negative limit
      wanted = limit = @limit.abs

      until @buffer.exhausted?
        available = @buffer.fill_from @io, @skip
        break unless available > 0

        if count = @buffer.find(@separator)
          bytes = count < wanted ? count : wanted
          str << @buffer.shift(bytes)

          str.taint
          $. = @io.increment_lineno
          @buffer.discard @skip if @skip

          yield str

          str = ""
          wanted = limit
        else
          if wanted < available
            str << @buffer.shift(wanted)

            str.taint
            $. = @io.increment_lineno
            @buffer.discard @skip if @skip

            yield str

            str = ""
            wanted = limit
          else
            str << @buffer.shift
            wanted -= available
          end
        end
      end

      str << @buffer.shift
      unless str.empty?
        str.taint
        $. = @io.increment_lineno
        yield str
      end
    end

    # Method G
    def read_all
      str = ""
      until @buffer.exhausted?
        @buffer.fill_from @io
        str << @buffer.shift
      end

      unless str.empty?
        str.taint
        $. = @io.increment_lineno
        yield str
      end
    end

    # Method H
    def read_to_limit
      str = ""
      wanted = limit = @limit.abs

      until @buffer.exhausted?
        available = @buffer.fill_from @io
        if wanted < available
          str << @buffer.shift(wanted)

          str.taint
          $. = @io.increment_lineno
          yield str

          str = ""
          wanted = limit
        else
          str << @buffer.shift
          wanted -= evailable
        end
      end
    end
  end

  def increment_lineno
    @lineno += 1
  end

  def each(sep_or_limit=$/, limit=nil, &block)
    return to_enum(:each, sep_or_limit, limit) unless block_given?

    ensure_open_and_readable

    if limit
      limit = Rubinius::Type.coerce_to limit, Integer, :to_int
      sep = sep_or_limit ? StringValue(sep_or_limit) : nil
    else
      case sep_or_limit
      when String
        sep = sep_or_limit
      when nil
        sep = nil
      else
        unless sep = Rubinius::Type.check_convert_type(sep_or_limit, String, :to_str)
          sep = $/
          limit = Rubinius::Type.coerce_to sep_or_limit, Integer, :to_int
        end
      end
    end

    return if @ibuffer.exhausted?

    EachReader.new(self, @ibuffer, sep, limit).each(&block)

    self
  end

  alias_method :each_line, :each

  def gets(sep_or_limit=$/, limit=nil)
    each sep_or_limit, limit do |line|
      $_ = line if line
      return line
    end

    nil
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
          str = ""
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

  def set_encoding(external, internal=nil)
    unless external.kind_of? Encoding or external.kind_of? String
      external = StringValue(external) if external
    end

    unless internal.kind_of? Encoding or internal.kind_of? String
      internal = StringValue(internal) if internal
    end

    if external.kind_of? String
      external, internal = external.split(':') unless internal
    end

    internal = nil if internal == "-"

    external = Encoding.find external if external.kind_of? String
    internal = Encoding.find internal if internal.kind_of? String

    @external = external
    @internal = internal unless internal == external
  end

  def external_encoding
    @external
  end

  def internal_encoding
    @internal
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
      elsif str.kind_of? Array
        if str.first.kind_of? Hash
          env = str.first
          env.each { |k, v| ENV[k] = v }
          cmd = str[1..-1]
        else
          cmd = str
        end
        Process.perform_exec cmd.first, cmd.map(&:to_s)
      else
        Process.perform_exec "/bin/sh", ["sh", "-c", str]
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
end

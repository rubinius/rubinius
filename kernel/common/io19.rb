# -*- encoding: us-ascii -*-

class IO
  attr_accessor :external
  attr_accessor :internal

  module WaitReadable; end
  module WaitWritable; end

  class EAGAINWaitReadable < Errno::EAGAIN
    include ::IO::WaitReadable
  end

  class EAGAINWaitWritable < Errno::EAGAIN
    include ::IO::WaitWritable
  end

  def self.binread(file, length=nil, offset=0)
    raise ArgumentError, "Negative length #{length} given" if !length.nil? && length < 0

    File.open(file, "rb") do |f|
      f.seek(offset)
      f.read(length)
    end
  end

  def self.binwrite(file, string, *args)
    offset, opts = args
    opts ||= {}
    if offset.is_a?(Hash)
      offset, opts = nil, offset
    end

    mode, binary, external, internal, autoclose = IO.normalize_options(nil, opts)
    unless mode
      mode = File::CREAT | File::RDWR | File::BINARY
      mode |= File::TRUNC unless offset
    end
    File.open(file, mode, :encoding => (external || "ASCII-8BIT")) do |f|
      f.seek(offset || 0)
      f.write(string)
    end
  end

  def self.write(file, string, *args)
    if args.size > 2
      raise ArgumentError, "wrong number of arguments (#{args.size + 2} for 2..3)"
    end

    offset, opts = args
    opts ||= {}
    if offset.is_a?(Hash)
      offset, opts = nil, offset
    end

    mode, binary, external, internal, autoclose = IO.normalize_options(nil, opts)
    unless mode
      mode = File::CREAT | File::RDWR
      mode |= File::TRUNC unless offset
    end

    open_args = opts[:open_args] || [mode, :encoding => (external || "ASCII-8BIT")]
    File.open(file, *open_args) do |f|
      f.seek(offset || 0)
      f.write(string)
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

      size = @length ? @length : 16384
      bytes = 0

      begin
        while data = @from.send(@method, size, "")
          @to.write data
          bytes += data.size

          break if @length && bytes >= @length
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

  def self.normalize_options(mode, options)
    mode = nil if mode.equal?(undefined)
    autoclose = true

    if options.equal?(undefined)
      options = Rubinius::Type.try_convert(mode, Hash, :to_hash)
      mode = nil if options
    elsif !options.nil?
      options = Rubinius::Type.try_convert(options, Hash, :to_hash)
      raise ArgumentError, "wrong number of arguments (3 for 1..2)" unless options
    end

    if mode
      mode = (Rubinius::Type.try_convert(mode, Integer, :to_int) or
              Rubinius::Type.coerce_to(mode, String, :to_str))
    end

    if options
      if optmode = options[:mode]
        optmode = (Rubinius::Type.try_convert(optmode, Integer, :to_int) or
                   Rubinius::Type.coerce_to(optmode, String, :to_str))
      end

      if mode
        raise ArgumentError, "mode specified twice" if optmode
      else
        mode = optmode
      end

      autoclose = !!options[:autoclose] if options.key?(:autoclose)
    end

    if mode.kind_of?(String)
      mode, external, internal = mode.split(":")
      raise ArgumentError, "invalid access mode" unless mode

      binary = true  if mode[1] === ?b
      binary = false if mode[1] === ?t
    elsif mode
      binary = true  if (mode & BINARY) != 0
    end

    if options
      if options[:textmode] and options[:binmode]
        raise ArgumentError, "both textmode and binmode specified"
      end

      if binary.nil?
        binary = options[:binmode]
      elsif options.key?(:textmode) or options.key?(:binmode)
        raise ArgumentError, "text/binary mode specified twice"
      end

      if !external and !internal
        external = options[:external_encoding]
        internal = options[:internal_encoding]
      elsif options[:external_encoding] or options[:internal_encoding] or options[:encoding]
        raise ArgumentError, "encoding specified twice"
      end

      if !external and !internal
        encoding = options[:encoding]
        external, internal = encoding.split(':') if encoding.kind_of? String
      end
    end

    [mode, binary, external, internal, autoclose]
  end

  #
  # Create a new IO associated with the given fd.
  #
  def initialize(fd, mode=undefined, options=undefined)
    if block_given?
      warn 'IO::new() does not take block; use IO::open() instead'
    end

    mode, binary, external, internal, @autoclose = IO.normalize_options(mode, options)

    IO.setup self, Rubinius::Type.coerce_to(fd, Integer, :to_int), mode

    binmode                          if binary
    set_encoding(external, internal) if external or internal
  end

  private :initialize

  def autoclose?
    @autoclose
  end

  def autoclose=(autoclose)
    @autoclose = !!autoclose
  end

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

  def each_char
    return to_enum(:each_char) unless block_given?

    ensure_open_and_readable
    if Rubinius.kcode == :UTF8
      # TODO zoinks. This is the slowest way possible to do this.
      # We'll have to rewrite it.
      lookup = 7.downto(4)
      while c = read(1) do
        n = c[0].ord
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
      if external.downcase.start_with? "bom|"
        use_bom = true
        external = external.gsub /\Abom\|/i, ""
      end
    end

    internal = nil if internal == "-"

    if use_bom && encoding = strip_bom
      external = encoding
    end

    external = Encoding.find external if external.kind_of? String
    internal = Encoding.find internal if internal.kind_of? String

    @external = external
    @internal = internal unless internal == external
  end

  def strip_bom
    chars = []
    chars << (c = getc)

    if c == 0x00
      chars << (c = getc)
      if c == 0x00
        chars << (c = getc)
        if c == 0xFE
          chars << (c = getc)
          if c == 0xFF
            return "UTF-32BE"
          end
        end
      end
    end

    if c == 0xFF
      chars << (c = getc)
      if c == 0xFE
        chars << (c = getc)
        if c == 0x00
          chars << (c = getc)
          if c == 0x00
            return "UTF-32LE"
          end
        else
          ungetc c
          return "UTF-16LE"
        end
      end
    end

    if c == 0xFE
      chars << (c = getc)
      if c == 0xFF
        return "UTF-16BE"
      end
    end

    if c == 0xEF
      chars << (c = getc)
      if c == 0xBB
        chars << (c = getc)
        if c == 0xBF
          return "UTF-8"
        end
      end
    end

    chars.reverse_each do |c|
      ungetc c
    end
    nil
  end

  def external_encoding
    @external
  end

  def internal_encoding
    @internal
  end

  def binmode
    ensure_open

    @binmode = true
    @external = Encoding::BINARY
    @internal = nil

    # HACK what to do?
    self
  end

  def binmode?
    !@binmode.nil?
  end

  def close_on_exec=(value)
    if value
      fcntl(F_SETFD, fcntl(F_GETFD) | FD_CLOEXEC)
    else
      fcntl(F_SETFD, fcntl(F_GETFD) & ~FD_CLOEXEC)
    end
    nil
  end

  def close_on_exec?
    (fcntl(F_GETFD) & FD_CLOEXEC) != 0
  end

  def self.pipe(external_encoding=nil, internal_encoding=nil)
    lhs = allocate
    rhs = allocate

    begin
      connect_pipe(lhs, rhs)
    rescue Errno::EMFILE
      GC.run(true)
      connect_pipe(lhs, rhs)
    end

    external_encoding ||= Encoding.default_external
    internal_encoding ||= Encoding.default_internal

    if external_encoding or internal_encoding
      lhs.set_encoding(external_encoding, internal_encoding)
    end

    lhs.sync = true
    rhs.sync = true

    if block_given?
      begin
        yield lhs, rhs
      ensure
        lhs.close unless lhs.closed?
        rhs.close unless rhs.closed?
      end
    else
      [lhs, rhs]
    end
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
  def self.popen(str, mode=undefined, options=undefined)
    mode, binary, external, internal, autoclose = IO.normalize_options(mode, options)
    mode = parse_mode(mode || 'r')

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

        if str.last.kind_of? Hash
          redirects, options = Rubinius::Spawn.adjust_options(str.pop)
          Rubinius::Spawn.setup_redirects(redirects)
          Rubinius::Spawn.setup_options(options)
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

    pipe.binmode                          if binary
    pipe.set_encoding(external, internal) if external or internal

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
    if @descriptor != -1
      "#<#{self.class}:fd #{@descriptor}>"
    else
      "#<#{self.class}:(closed)"
    end
  end

  def lines(*args, &block)
    if block_given?
      each_line(*args, &block)
    else
      to_enum :each_line, *args
    end
  end

  def advise(advice, offset = 0, len = 0)
    raise IOError, "stream is closed" if closed?
    raise TypeError, "advice must be a Symbol" unless advice.kind_of?(Symbol)

    if offset.kind_of?(Bignum) || len.kind_of?(Bignum)
      raise RangeError, "bignum too big to convert into `long'"
    end

    unless [:normal, :sequential, :random, :noreuse, :dontneed, :willneed].include? advice
      raise NotImplementedError, "Unsupported advice: #{advice}"
    end

    offset = Rubinius::Type.coerce_to offset, Integer, :to_int
    len = Rubinius::Type.coerce_to len, Integer, :to_int

    Rubinius.primitive :io_advise
    nil
  end
end

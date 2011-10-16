class IO
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

    unless length.equal?(undefined)
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
  #  f.each {|line| puts "#{f.lineno}: #{line}" }
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
end

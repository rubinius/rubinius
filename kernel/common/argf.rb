# -*- encoding: us-ascii -*-



module Rubinius

  # :internal:
  #
  # The virtual concatenation file of the files given on command line (or
  # from $stdin if no files were given.)
  #
  # The only instance hereof is the object referred to by ARGF.
  #
  # @see  ARGF
  #
  class ARGFClass
    include Enumerable

    # :internal:
    #
    # Create a stateless ARGF.
    #
    # The actual setup is done on the fly:
    #
    # @see  #advance!
    #
    def initialize
      @lineno = 0
      @advance = true
      @init = false
      @use_stdin_only = false
    end

    #
    # Set stream into binary mode.
    #
    # Stream is set into binary mode, i.e. 8-bit ASCII.
    # Once set, the binary mode cannot be undone. Returns
    # self.
    #
    # @todo Not implemented! Intentional? --rue
    #
    def binmode
      self
    end

    #
    # Close stream.
    #
    def close
      advance!
      @stream.close

      @advance = true unless @use_stdin_only
      @lineno = 0

      self
    end

    #
    # True if the stream is closed.
    #
    def closed?
      advance!
      @stream.closed?
    end

    #
    # Linewise iteration.
    #
    # Yields one line from stream at a time, as given by
    # #gets. An Enumerator is returned if no block is
    # provided. Returns nil if no content, self otherwise.
    #
    # @see  #gets.
    #
    def each_line(sep=$/)
      return to_enum :each_line, sep unless block_given?
      return nil unless advance!

      while line = gets(sep)
        yield line
      end
      self
    end
    alias_method :lines, :each_line
    alias_method :each, :each_line

    #
    # Bytewise iteration.
    #
    # Yields one byte at a time from stream, an Integer
    # as given by #getc. An Enumerator is returned if no
    # block is provided. Returns self.
    #
    # @see  #getc
    #
    def each_byte
      return to_enum :each_byte unless block_given?
      while ch = getbyte()
        yield ch
      end
      self
    end
    alias_method :bytes, :each_byte

    #
    # Character-wise iteration.
    #
    # Yields one character at a time from stream. An
    # Enumerator is returned if no block is provided.
    # Returns self.
    #
    # The characters yielded are gotten from #getc.
    #
    # @see  #getc
    #
    def each_char
      return to_enum :each_char unless block_given?
      while c = getc()
        yield c.chr
      end
      self
    end
    alias_method :chars, :each_char

    #
    # Query whether stream is at end-of-file.
    #
    # True if there is a stream and it is in EOF
    # status.
    #
    def eof?
      @stream and @stream.eof?
    end
    alias_method :eof, :eof?

    #
    # File descriptor number for stream.
    #
    # Returns a file descriptor number for the stream being
    # read out of.
    #
    # @todo   Check correctness, does this imply there may be
    #         multiple FDs and if so, is this correct? --rue
    #
    def fileno
      raise ArgumentError, "No stream" unless advance!
      @stream.fileno
    end
    alias_method :to_i, :fileno

    #
    # File path currently in use.
    #
    # Path to file from which read currently is
    # occurring, or an indication that the stream
    # is STDIN.
    #
    def filename
      advance!
      @filename
    end
    alias_method :path, :filename

    #
    # Current stream object.
    #
    # This may change during the course of execution,
    # but is the current one!
    #
    def file
      advance!
      @stream
    end

    def getbyte
      while true
        return nil unless advance!
        if val = @stream.getbyte
          return val
        end

        return nil if @use_stdin_only
        @stream.close unless @stream.closed?
        @advance = true
      end
    end

    #
    # Return one character from stream.
    #
    # If a character cannot be returned and we are
    # reading from a file, the stream is closed.
    #
    def getc
      while true
        return nil unless advance!
        if val = @stream.getc
          return val
        end

        return nil if @use_stdin_only
        @stream.close unless @stream.closed?
        @advance = true
      end
    end

    #
    # Return next line of text from stream.
    #
    # If a line cannot be returned and we are
    # reading from a file, the stream is closed.
    #
    # The mechanism does track the line numbers,
    # and updates $. accordingly.
    #
    def gets(sep=$/)
      while true
        return nil unless advance!
        line = @stream.gets(sep)

        unless line
          return nil if @use_stdin_only
          @stream.close unless @stream.closed?
          @advance = true
          next
        end

        @lineno += 1
        $. = @lineno
        return line
      end
    end

    #
    # Return current line number.
    #
    # Line numbers are maintained when using the linewise
    # access methods.
    #
    # @see  #gets
    # @see  #each_line
    #
    attr_reader :lineno

    #
    # Set current line number.
    #
    # Also sets $. accordingly.
    #
    # @todo Should this be public? --rue
    #
    def lineno=(val)
      $. = @lineno = val
    end

    #
    # Return stream position for seeking etc.
    #
    # @see IO#pos.
    #
    def pos
      raise ArgumentError, "no stream" unless advance!
      @stream.tell
    end
    alias_method :tell, :pos

    #
    # Set stream position to a previously obtained position.
    #
    # @see IO#pos=
    #
    def pos=(position)
      raise ArgumentError, "no stream" unless advance!
      @stream.pos = position
    end

    #
    # Read a byte from stream.
    #
    # Similar to #getc, but raises an EOFError if
    # EOF has been reached.
    #
    # @see  #getc
    #
    def readbyte
      advance!

      if val = getc()
        return val
      end

      raise EOFError, "ARGF at end"
    end
    alias_method :readchar, :readbyte

    #
    # Read number of bytes or all, optionally into buffer.
    #
    # If number of bytes is not given or is nil, tries to read
    # all of the stream, which is then closed. If the number is
    # specified, then at most that many bytes will be read.
    #
    # A buffer responding to #<< may be provided as the second
    # argument. The data read is pushed into it. If no buffer
    # is provided, as by default, a String with the data is
    # returned instead.
    #
    def read(bytes=nil, output=nil)
      # The user might try to pass in nil, so we have to check here
      output ||= ""

      if bytes
        bytes_left = bytes

        until bytes_left == 0
          return output unless advance!

          if res = @stream.read(bytes_left)
            output << res
            bytes_left -= res.size
          else
            break if @use_stdin_only
            @stream.close unless @stream.closed?
            @advance = true
          end

        end

        return output
      end

      while advance!
        output << @stream.read

        break if @use_stdin_only
        @stream.close unless @stream.closed?
        @advance = true
      end

      output
    end

    #
    # Read next line of text.
    #
    # As #gets, but an EOFError is raised if the stream
    # is at EOF.
    #
    # @see  #gets
    #
    def readline(sep=$/)
      raise EOFError, "ARGF at end" unless advance!

      if line = gets(sep)
        return line
      end

      raise EOFError, "ARGF at end"
    end

    #
    # Rewind the stream to its beginning.
    #
    # Line number is updated accordingly.
    #
    # @todo Is this correct, only current stream is rewound? --rue
    #
    def rewind
      raise ArgumentError, "no stream to rewind" unless advance!
      @lineno -= @stream.lineno
      @stream.rewind
    end

    #
    # Seek into a previous position in the stream.
    #
    # @see IO#seek.
    #
    def seek(*args)
      raise ArgumentError, "no stream" unless advance!
      @stream.seek(*args)
    end

    #
    # Close file stream and return self.
    #
    # STDIN is not closed if being used, otherwise the
    # stream gets closed. Returns self.
    #
    def skip
      return self if @use_stdin_only
      @stream.close unless @stream.closed?
      @advance = true
      self
    end

    #
    # Return IO object for current stream.
    #
    # @see IO#to_io
    #
    def to_io
      advance!
      @stream.to_io
    end

    #
    # Returns "ARGF" as the string representation of this object.
    #
    def to_s
      "ARGF"
    end


    # Internals

    #
    # Main processing.
    #
    # If not initialised yet, sets the object up on either
    # first of provided file names or STDIN.
    #
    # Does nothing further or later if using STDIN, but if
    # there are further file names in ARGV, tries to open
    # the next one as the current stream.
    #
    def advance!
      return true unless @advance

      unless @init

        if ARGV.empty?
          @advance = false
          @stream = STDIN
          @filename = "-"
          @use_stdin_only = true
          return true
        end

        @init = true
      end

      File.unlink(@backup_filename) if @backup_filename && $-i == ""

      return false if @use_stdin_only || ARGV.empty?

      @advance = false

      file = ARGV.shift
      @stream = (file == "-" ? STDIN : File.open(file, "r"))
      @filename = file

      if $-i && @stream != STDIN
        backup_extension = $-i == "" ? ".bak" : $-i
        @backup_filename = "#{@filename}#{backup_extension}"
        File.rename(@filename, @backup_filename)
        @stream = File.open(@backup_filename, "r")
        $stdout = File.open(@filename, "w")
      end

      return true
    end
    private :advance!

  end
end

#
# The virtual concatenation file of the files given on command line (or
# from $stdin if no files were given.) Usable like an IO.
#
ARGF = Rubinius::ARGFClass.new


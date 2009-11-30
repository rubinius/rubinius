##
# The virtual concatenation file of the files given on command line (or from
# $stdin if no files were given).

module Rubinius
  class ARGFClass
    include Enumerable

    def initialize
      @lineno = 0
      @advance = true
      @init = false
      @use_stdin_only = false
    end

    def bytes(&b)
      return to_enum(:each_byte) unless block_given?
      each_byte(&b)
      self
    end

    def binmode
      self
    end

    def chars(&b)
      return to_enum(:each_char) unless block_given?
      each_char(&b)
      self
    end

    def close
      advance!
      @stream.close

      @advance = true unless @use_stdin_only
      @lineno = 0

      self
    end

    def closed?
      advance!
      @stream.closed?
    end

    def each_line
      return to_enum :each_line unless block_given?
      return nil unless advance!

      while line = gets()
        yield line
      end
      self
    end

    def each(&block)
      return to_enum :each unless block_given?
      each_line &block
      self
    end

    def each_byte
      return to_enum :each_byte unless block_given?
      while ch = getc()
        yield ch
      end
      self
    end

    def each_char(&block)
      return to_enum :each_char unless block_given?
      while c = getc()
        yield c.chr
      end
      self
    end

    def eof?
      @stream and @stream.eof?
    end

    alias_method :eof, :eof?

    def fileno
      raise ArgumentError, "no stream" unless advance!
      @stream.fileno
    end

    alias_method :to_i, :fileno

    def filename
      advance!
      @filename
    end

    alias_method :path, :filename

    def file
      advance!
      @stream
    end

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
    alias_method :getbyte, :getc

    def gets
      while true
        return nil unless advance!
        line = @stream.gets

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

    def lines(*args)
      to_enum :each_line, *args
    end

    attr_reader :lineno

    def lineno=(val)
      $. = @lineno = val
    end

    def pos
      raise ArgumentError, "no stream" unless advance!
      @stream.tell
    end

    alias_method :tell, :pos

    def pos=(position)
      raise ArgumentError, "no stream" unless advance!
      @stream.pos = position
    end

    def readbyte
      advance!

      if val = getc()
        return val
      end

      raise EOFError, "ARGF at end"
    end

    alias_method :readchar, :readbyte

    def read(bytes=nil, output=nil)
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

      return output
    end

    def readlines
      return nil unless advance!

      lines = []
      while line = gets()
        lines << line
      end
      return lines
    end

    def readline
      raise EOFError, "ARGF at end" unless advance!

      if line = gets()
        return line
      end

      raise EOFError, "ARGF at end"
    end

    alias_method :to_a, :readlines

    def rewind
      raise ArgumentError, "no stream to rewind" unless advance!
      @lineno -= @stream.lineno
      @stream.rewind
    end

    def seek(*args)
      raise ArgumentError, "no stream" unless advance!
      @stream.seek(*args)
    end

    def skip
      return self if @use_stdin_only
      @stream.close unless @stream.closed?
      @advance = true
      self
    end

    def to_io
      advance!
      @stream.to_io
    end

    def to_s
      "ARGF"
    end

    ## private methods

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

      return false if @use_stdin_only || ARGV.empty?

      @advance = false

      file = ARGV.shift
      @stream = (file == "-" ? STDIN : File.open(file, "r"))
      $FILENAME = @filename = file

      return true
    end
    private :advance!

  end
end

ARGF = Rubinius::ARGFClass.new

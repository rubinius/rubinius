class ARGF
  def self.getc
    stream = next_stream
    stream.getc if stream
  end

  def self.gets
    stream = next_stream
    stream.gets if stream
  end

  def self.each_line(&block)
    while stream = next_stream
      stream.each_line(&block)
    end
  end

  def self.each(&block)
    each_line &block
  end

  def self.each_byte(&block)
    while stream = next_stream
      stream.each_byte(&block)
    end
  end

  def self.read(bytes=nil, output=nil)
    result = ""
    have_enough = false
    while not have_enough and stream = next_stream
      to_read = bytes ? (bytes-result.size) : nil
      result << stream.read(to_read)
      have_enough = bytes ? (result.size == bytes) : false
    end
    output ? output << result : result
  end

  def self.readlines
    lines = []
    while stream = next_stream
      lines << stream.gets
    end
    lines.any? ? lines : nil
  end

  def self.to_a
    readlines
  end

  def self.readline
    stream = next_stream
    raise EOFError unless stream
    stream.readline
  end

  def self.readchar
    stream = next_stream
    raise EOFError unless stream
    stream.readchar
  end

  def self.pos
    raise ArgumentError if closed?
    current_stream.pos
  end

  def self.tell
    pos
  end

  def self.pos=(position)
    current_stream.pos = position unless current_stream.closed?
  end

  def self.seek(offset, whence=IO::SEEK_SET)
    current_stream.seek(offset, whence) unless closed? or current_stream.closed?
  end

  def self.rewind
    raise ArgumentError if closed?
    current_stream.rewind
  end

  def self.fileno
    raise ArgumentError if closed?
    @fileno
  end

  def self.to_i
    fileno
  end

  def self.filename
    current_stream
    @last_filename
  end

  def self.path
    filename
  end

  def self.file
    current_stream
    @last_file
  end

  def self.to_io
    current_stream.to_io
  end

  def self.skip
    current_stream.close unless closed? or current_stream.closed?
  end

  def self.eof
   raise IOError if closed?
   current_stream.eof
  end

  def self.eof?
    eof
  end

  def self.close
    close_stream if current_stream
  end

  def self.closed?
    ARGV.empty? and @stream == nil
  end

  private def self.current_stream
    @stream or next_stream
  end

  private def self.next_stream
    return @stream unless @stream == nil or @stream.closed? or @stream.eof?

    close_stream if @stream
    return nil if ARGV.empty?

    @last_filename = ARGV.shift
    $FILENAME = @last_filename
    @stream = @last_filename == '-' ? STDIN : File.open(@last_filename, 'r')
    @last_file = @stream
    @fileno = @fileno || 0 + 1

    @stream
  end

  private def self.close_stream
    @stream.close unless @stream.closed? or @stream.fileno == 0 # STDIN
    @stream = nil if ARGV.empty?
  end
end

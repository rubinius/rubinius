module FileSpecs
  # Try to set up known locations of each filetype
  def self.reconfigure()
    @file   = tmp("test.txt")
    @dir    = Dir.pwd
    @fifo   = tmp("test_fifo")
    @sock   = nil

    platform_is_not :windows do
      @block  = `find /dev /devices -type b 2> /dev/null`.split("\n").first
      @char   = `find /dev /devices -type c 2> /dev/null`.split("\n").last

      %w[/dev /usr/bin /usr/local/bin].each do |dir|
        links = `find #{dir} -type l 2> /dev/null`.split("\n")
        next if links.empty?
        @link = links.first
        break
      end

      find_socket
    end
  end

  # TODO: This is probably too volatile
  def self.find_socket()
    %w[/tmp /var/run].each do |dir|
      socks = `find #{dir} -type s 2> /dev/null`.split("\n")
      next if socks.empty?
      @sock = socks.first
      break
    end
  end

  # TODO: Automatic reload mechanism
  reconfigure

  def self.normal_file()
    File.open(@file, "w") {} # 'Touch'
    yield @file
  ensure
    File.unlink @file
  end

  def self.directory()
    yield @dir
  end

  def self.fifo()
    system "mkfifo #{@fifo} 2> /dev/null"
    yield @fifo
  ensure
    File.unlink @fifo
  end

  def self.block_device()
    yield @block
  end

  def self.character_device()
    yield @char
  end

  def self.symlink()
    yield @link
  end

  # This will silently not execute the block if no socket
  # can be found. However, if you are running X, there is
  # a good chance that if nothing else, at least the X
  # Server socket exists.
  def self.socket()
    find_socket
    yield @sock if @sock
  end
end

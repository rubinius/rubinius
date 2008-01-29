module FileSpecs
  # Try to set up known locations of each filetype
  def self.reconfigure()
    @file   = "test.txt"
    @dir    = Dir.pwd
    @fifo   = "test_fifo"
    @block  = `find /dev -type b 2> /dev/null`.split("\n").first
    @char   = `find /dev -type c 2> /dev/null`.split("\n").first

    %w[/dev /usr/bin /usr/local/bin].each do |dir|
      links = `find #{dir} -type l 2> /dev/null`.split("\n")
      next if links.empty?
      @link = links.first
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
end

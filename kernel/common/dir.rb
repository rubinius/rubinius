class Dir
  include Enumerable

  def self.[](*patterns)
    if patterns.size == 1
      patterns = StringValue(patterns[0]).split("\0")
    end

    files = []

    patterns.each do |pat|
      Dir::Glob.glob pat, 0, files
    end

    files
  end

  def self.glob(pattern, flags=0, &block)
    if pattern.kind_of? Array
      patterns = pattern
    else
      pattern = StringValue pattern

      return [] if pattern.empty?

      patterns = pattern.split("\0")
    end

    matches = []

    patterns.each do |pat|
      Dir::Glob.glob pat, flags, matches
    end

    if block
      matches.each(&block)
      return nil
    end

    return matches
  end

  def self.foreach(path)
    return to_enum(:foreach, path) unless block_given?

    open(path) do |dir|
      while s = dir.read
        yield s
      end
    end

    nil
  end

  def self.join_path(p1, p2, dirsep)
    "#{p1}#{dirsep ? '/' : ''}#{p2}"
  end

  def self.chdir(path = ENV['HOME'])
    path = StringValue path

    if block_given?
      original_path = self.getwd
      error = FFI::Platform::POSIX.chdir path
      Errno.handle(path) if error != 0

      begin
        value = yield path
      ensure
        error = FFI::Platform::POSIX.chdir original_path
        Errno.handle(original_path) if error != 0
      end

      return value
    else
      error = FFI::Platform::POSIX.chdir path
      if error != 0
        Errno.handle path
      end
      error
    end
  end

  def self.mkdir(path, mode = 0777)
    error = FFI::Platform::POSIX.mkdir(StringValue(path), mode)
    if error != 0
      Errno.handle path
    end
    error
  end

  def self.rmdir(path)
    error = FFI::Platform::POSIX.rmdir(StringValue(path))
    if error != 0
      Errno.handle path
    end
    error
  end

  def self.getwd
    buf = " " * 1024
    FFI::Platform::POSIX.getcwd(buf, buf.length)
  end

  def self.open(path)
    dir = new path
    if block_given?
      begin
        value = yield dir
      ensure
        dir.close
      end

      return value
    else
      return dir
    end
  end

  def self.entries(path)
    ret = []

    open(path) do |dir|
      while s = dir.read
        ret << s
      end
    end

    ret
  end

  def self.chroot(path)
    ret = FFI::Platform::POSIX.chroot StringValue(path)
    Errno.handle path
    return ret
  end

  def each
    return to_enum unless block_given?

    while s = read
      yield s
    end

    self
  end

  attr_reader :path

  SeekKind = 0
  RewindKind = 1
  TellKind = 2

  def pos
    control TellKind, 0
  end

  alias_method :tell, :pos

  def pos=(position)
    seek(position)

    position
  end

  def seek(position)
    control SeekKind, position

    self
  end

  def rewind
    control RewindKind, 0

    self
  end

  def inspect
    "#<#{self.class}:#{object_id.to_s(16)} @path=#{@path}>"
  end

  class << self
    alias_method :pwd, :getwd
    alias_method :delete, :rmdir
    alias_method :unlink, :rmdir
  end
end

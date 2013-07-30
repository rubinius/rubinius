# -*- encoding: us-ascii -*-

class Dir
  include Enumerable

  FFI = Rubinius::FFI

  def self.[](*patterns)
    if patterns.size == 1
      pattern = Rubinius::Type.coerce_to_path(patterns[0])
      return [] if pattern.empty?
      patterns = glob_split pattern
    end

    glob patterns
  end

  #   files = []
  #   index = 0

  #   patterns.each do |pat|
  #     enc = Rubinius::Type.ascii_compatible_encoding pat

  #     Dir::Glob.glob pat, 0, files

  #     total = files.size
  #     while index < total
  #       Rubinius::Type.encode_string files[index], enc
  #       index += 1
  #     end
  #   end

  #   files
  # end

  def self.glob(pattern, flags=0, &block)
    if pattern.kind_of? Array
      patterns = pattern
    else
      pattern = Rubinius::Type.coerce_to_path pattern

      return [] if pattern.empty?

      patterns = glob_split pattern
    end

    matches = []
    index = 0

    patterns.each do |pat|
      enc = Rubinius::Type.ascii_compatible_encoding pat
      Dir::Glob.glob pat, flags, matches

      total = matches.size
      while index < total
        Rubinius::Type.encode_string matches[index], enc
        index += 1
      end
    end

    if block
      matches.each(&block)
      return nil
    end

    return matches
  end

  def self.glob_split(pattern)
    result = []
    start = 0
    while idx = pattern.find_string("\0", start)
      result << pattern.byteslice(start, idx)
      start = idx + 1
    end
    result << pattern.byteslice(start, pattern.bytesize)
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
    path = Rubinius::Type.coerce_to_path path

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
      Errno.handle path if error != 0
      error
    end
  end

  def self.mkdir(path, mode = 0777)
    error = FFI::Platform::POSIX.mkdir(Rubinius::Type.coerce_to_path(path), mode)
    Errno.handle path if error != 0
    error
  end

  def self.rmdir(path)
    error = FFI::Platform::POSIX.rmdir(Rubinius::Type.coerce_to_path(path))
    Errno.handle path if error != 0
    error
  end

  def self.getwd
    buf = String.pattern Rubinius::PATH_MAX, 0
    wd = FFI::Platform::POSIX.getcwd(buf, buf.length)
    Errno.handle unless wd
    Rubinius::Type.external_string wd
  end

  def self.chroot(path)
    ret = FFI::Platform::POSIX.chroot Rubinius::Type.coerce_to_path(path)
    Errno.handle path if ret != 0
    ret
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

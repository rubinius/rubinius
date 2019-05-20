class Dir
  include Enumerable

  def self.allocate
    Rubinius.primitive :dir_allocate
    raise PrimitiveFailure, "Dir.allocate primitive failed"
  end

  def initialize(path, options=undefined)
    path = Rubinius::Type.coerce_to_path path

    if options.equal? undefined
      enc = nil
    else
      options = Rubinius::Type.coerce_to options, Hash, :to_hash
      enc = options[:encoding]
      enc = Rubinius::Type.coerce_to_encoding enc if enc
    end

    Rubinius.invoke_primitive :dir_open, self, path, enc
  end

  private :initialize

  def close
    Rubinius.primitive :dir_close
    raise PrimitiveFailure, "Dir#close primitive failed"
  end

  def closed?
    Rubinius.primitive :dir_closed_p
    raise PrimitiveFailure, "Dir#closed? primitive failed"
  end

  def read
    entry = Rubinius.invoke_primitive :dir_read, self
    return unless entry

    if Encoding.default_external == Encoding::US_ASCII && !entry.valid_encoding?
      entry.force_encoding Encoding::ASCII_8BIT
      return entry
    end

    enc = Encoding.default_internal
    enc ? entry.encode(enc) : entry
  end

  def control(kind, pos)
    Rubinius.primitive :dir_control
    raise PrimitiveFailure, "Dir#__control__ primitive failed"
  end

  private :control

  def self.open(path, options=undefined)
    dir = new path, options
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

  def self.entries(path, options=undefined)
    ret = []

    open(path, options) do |dir|
      while s = dir.read
        ret << s
      end
    end

    ret
  end

  def self.exist?(path)
    PrivateFile.directory?(path)
  end

  class << self
    alias_method :exists?, :exist?
  end

  def self.home(user=nil)
    PrivateFile.expand_path("~#{user}")
  end

  def self.[](*patterns)
    if patterns.size == 1
      pattern = Rubinius::Type.coerce_to_path(patterns[0])
      return [] if pattern.empty?
      patterns = glob_split pattern
    end

    glob patterns
  end

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
      pat = Rubinius::Type.coerce_to_path pat
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
    m = Rubinius::Mirror.reflect pattern
    while idx = m.find_string("\0", start)
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

  def fileno
    Rubinius.primitive :dir_fileno
    raise PrimitiveFailure, "Dir#fileno primitive failed"
  end

  attr_reader :path

  alias_method :to_path, :path

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

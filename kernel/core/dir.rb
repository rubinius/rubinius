# depends on: array.rb

class Dir
  class DirEntry < FFI::Struct            
    # struct dirent {
    #   ino_t d_ino;      /* file number of entry */
    #   __uint16_t d_reclen;    /* length of this record */
    #   __uint8_t  d_type;    /* file type, see below */
    #   __uint8_t  d_namlen;    /* length of string in d_name */
    #   char d_name[__DARWIN_MAXNAMLEN + 1];  /* name must be no longer than this */
    # };
    
    layout  :d_ino,    :uint,
            :d_reclen, :ushort,
            :d_type,   :uchar,
            :d_namlen, :uchar,
            :d_name,   :char_array
  end

  include Enumerable
  
  def self.glob(pattern, flags = 0)
    prim_glob pattern, flags
  end

  def self.prim_glob(pattern, flags)
    Ruby.primitive :dir_glob
  end
  
  def self.[](pattern)
    glob(pattern, 0)
  end
  
  def self.chdir(path = ENV['HOME'])
    if block_given?
      original_path = self.getwd
      Platform::POSIX.chdir path
      value = yield path
      Platform::POSIX.chdir original_path
      return value
    else
      error = Platform::POSIX.chdir path
      if error != 0
        Errno.handle path
      end
      error
    end
  end
  
  def self.mkdir(path, mode = 0777)
    error = Platform::POSIX.mkdir(path, mode)
    if error != 0
      Errno.handle path
    end
    error
  end
  
  def self.rmdir(path)
    error = Platform::POSIX.rmdir(path)
    if error != 0
      Errno.handle path
    end
    error
  end

  def self.getwd
    buf = " " * 1024
    Platform::POSIX.getcwd(buf, buf.length)
  end
  
  def self.open(path)
    dir = self.new(path)
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

    self.open(path) do |dir|
      while s = dir.read
        ret << s
      end
    end

    ret
  end
  
  def self.foreach(path)
    self.open(path) do |dir|
      while s = dir.read
        yield s
      end
    end

    nil
  end

  def initialize(path)
    @dirptr = Platform::POSIX.opendir(path)

    if @dirptr.null?
      Errno.handle path
    end

    @path = path
  end
  
  def path
    raise IOError, "closed directory" if @dirptr.nil?

    @path
  end
  
  def close
    raise IOError, "closed directory" if @dirptr.nil?

    Platform::POSIX.closedir(@dirptr)
    @dirptr = nil
  end
  
  def read
    raise IOError, "closed directory" if @dirptr.nil?

    dir_entry_ptr = Platform::POSIX.readdir(@dirptr)
    return nil if dir_entry_ptr.null?
    DirEntry.new(dir_entry_ptr)[:d_name]
  end

  def each
    raise IOError, "closed directory" if @dirptr.nil?

    while s = read
      yield s
    end

    self
  end

  def pos
    raise IOError, "closed directory" if @dirptr.nil?
  end

  alias_method :tell, :pos

  def pos=(position)
    raise IOError, "closed directory" if @dirptr.nil?

    seek(position)
    position
  end

  def seek(position)
    raise IOError, "closed directory" if @dirptr.nil?

    self
  end

  def rewind
    raise IOError, "closed directory" if @dirptr.nil?

    Platform::POSIX.rewinddir(@dirptr)
    self
  end
  
  class << self
    alias_method :pwd, :getwd
    alias_method :delete, :rmdir
    alias_method :unlink, :rmdir
  end
end

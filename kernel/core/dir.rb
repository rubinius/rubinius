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
  
  def self.glob(pattern, flags)
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
        Errno.handle "Couldn't change to directory #{path}"
      end
      error
    end
  end
  
  def self.mkdir(path, mode = 0777)
    error = Platform::POSIX.mkdir(path, mode)
    if error != 0
      Errno.handle "Couldn't make directory #{path}"
    end
  end
  
  def self.rmdir(path)
    error = Platform::POSIX.rmdir(path)
    if error != 0
      Errno.handle "Couldn't delete directory #{path}"
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
      value = yield dir
      dir.close
      return value
    else
      return dir
    end
  end
  
  def initialize(path)
    @dirptr = Platform::POSIX.opendir(path)
    @path = path
  end
  
  def close
    # TODO: Fix this. When using the FFI call, it double frees.
    # I think this is happening in the DIR* internal struct
    # In the meantime, the method def means the read specs can run,
    # and the close specs fail instead of everything blowing up. -- KC 9/29/07
    # Platform::POSIX.closedir(@dirptr)
  end
  
  def read
    dir_entry_ptr = Platform::POSIX.readdir(@dirptr)
    return nil if dir_entry_ptr.null?
    DirEntry.new(dir_entry_ptr)[:d_name]
  end
  
  class << self
    alias_method :pwd, :getwd
    alias_method :delete, :rmdir
    alias_method :unlink, :rmdir
  end
end

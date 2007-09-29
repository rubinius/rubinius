class Dir
  module Foreign
    attach_function nil, 'chdir', [:string], :int
    
    attach_function nil, 'opendir',  [:string], :pointer
    attach_function nil, 'readdir',  [:pointer], :pointer
    attach_function nil, 'closedir', [:pointer], :int
  end
  
  # class DirEntry < FFI::Struct            
  #   # struct dirent {
  #   #   ino_t d_ino;      /* file number of entry */
  #   #   __uint16_t d_reclen;    /* length of this record */
  #   #   __uint8_t  d_type;    /* file type, see below */
  #   #   __uint8_t  d_namlen;    /* length of string in d_name */
  #   #   char d_name[__DARWIN_MAXNAMLEN + 1];  /* name must be no longer than this */
  #   # };
  #   
  #   layout :d_ino,    :uint,
  #          :d_reclen, :ushort,
  #          :d_type,   :uchar,
  #          :d_namlen, :uchar,
  #          :d_name,   :strptr    
  # end
  
  def self.glob(pattern, flags)
    Ruby.primitive :dir_glob
  end
  
  def self.[](pattern)
    glob(pattern, 0)
  end
  
  def self.chdir(path = ENV['HOME'])
    if block_given?
      original_path = self.getwd
      Foreign.chdir path
      value = yield path
      Foreign.chdir original_path
      return value
    else
      Foreign.chdir path
    end
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
    @dirptr = Foreign.opendir(path)
    @path = path
  end
  
  def close
    Foreign.closedir(@dirptr)
  end
  
  def read
    # TODO: DirEntry should subclass FFI::Struct instead of using
    # the struct constructor. Can't do it right now because splats are
    # broken, so FFI::Struct.layout is broken -- KC, 9/29/07
    dir_entry_ptr = Foreign.readdir(@dirptr)
    
    return nil if dir_entry_ptr.null?
    
    s = FFI::Struct.new(dir_entry_ptr, 
           :d_ino,    :uint,
           :d_reclen, :ushort,
           :d_type,   :uchar,
           :d_namlen, :uchar,
           :d_name,   :char_array)
    s[:d_name]
  end

  class << self
    alias_method :pwd, :getwd
  end
end

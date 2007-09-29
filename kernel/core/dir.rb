class Dir
  module Foreign
    attach_function nil, 'chdir', [:string], :int
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

  class << self
    alias_method :pwd, :getwd
  end
end

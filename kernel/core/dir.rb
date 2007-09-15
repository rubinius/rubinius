class Dir
  def self.glob(pattern, flags)
    Ruby.primitive :dir_glob
  end
  
  def self.[](pattern)
    glob(pattern, 0)
  end
  
  def self.chdir(path)
    Ruby.primitive :dir_chdir
  end

  def self.getwd
    buf = " " * 1024
    Platform::POSIX.getcwd(buf, buf.length)
  end

  class << self
    alias_method :pwd, :getwd
  end
end

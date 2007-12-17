# depends on: array.rb

class Dir
  class DirEntry < FFI::Struct
    layout :d_name, :char_array, Rubinius::RUBY_CONFIG['rbx.platform.dir.d_name']
  end

  include Enumerable
  
  GLOB_RECURSIVE = -2
  
  def self.[](pattern)
    glob(pattern, 0)
  end
  
  def self.glob(pattern, flags = 0)
    original_dir = Dir.pwd
    files = Array.new 0
    
    period = (flags & File::FNM_DOTMATCH) > 0
    period_excludes = ["..", "."]
    
    max_depth = glob_max_depth(pattern)
    
    Dir.foreach(original_dir) do |entry|
      if match = glob_match(pattern, entry, flags)
        files << match
      end
      
      if File.directory?(entry) and (max_depth == GLOB_RECURSIVE or max_depth > 0) and
         !period_excludes.include?(entry)
        next if entry =~ /^\./ and !period
        search_deeper(files, entry, pattern, flags, 1, max_depth)
      end
    end
    
    files.to_a
  end
  
  def self.search_deeper(files, path, pattern, flags, depth, max_depth) # needs depth
    period = (flags & File::FNM_DOTMATCH) > 0
    period_excludes = ["..", "."]
    
    paths = [[path,depth]]
    until paths.empty?
      
      path, depth = paths.shift
      Dir.foreach(path) do |entry|
        next if period_excludes.include?(entry)
        file = File.join(path, entry)
      
        if match = glob_match(pattern, file, flags)
          files << match
        end
      
        if File.directory?(file) and (max_depth == GLOB_RECURSIVE or depth < max_depth)
          next if entry =~ /^\./ and !period
          paths << [file, depth + 1]
        end
      end
      
    end
  end
  
  def self.glob_match(pattern, entry, flags)
    if glob_fnmatch? pattern, entry, flags
      entry
    elsif File.directory?(entry) and glob_fnmatch?(pattern, "#{entry}/", flags)
      "#{entry}/"
    else
      nil
    end
  end
  
  # This is going to need to be modified for {}
  def self.glob_fnmatch?(pattern, entry, flags)
    File.fnmatch?(pattern, entry, flags)
  end
  
  def self.glob_max_depth(pattern)
    # TODO: Change to check for path seperator rather than /
    if pattern =~ /\*{2}/
      # Special case for Dir["**"]
      seps = pattern.split("/").size - 1
      pattern_ends_in_slash = pattern[pattern.size - 1] == ?/
      if seps == 0 and !pattern_ends_in_slash
        max_depth = 0
      else
        max_depth = GLOB_RECURSIVE
      end
    else
      max_depth = pattern.split("/").size - 1
    end
  end
  
  def self.chdir(path = ENV['HOME'])
    if block_given?
      original_path = self.getwd
      error = Platform::POSIX.chdir path
      Errno.handle(path) if error != 0

      begin
        value = yield path
      ensure
        error = Platform::POSIX.chdir original_path
        Errno.handle(original_path) if error != 0
      end

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

    if @dirptr.nil?
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
    return nil if dir_entry_ptr.nil?
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

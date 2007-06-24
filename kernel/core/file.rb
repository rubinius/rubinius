class File < IO
  class FileError < Exception; end
  class NoFileError < FileError; end
  class UnableToStat < FileError; end
  class PermissionError < FileError; end

  if Rubinius::OS == :win32
    SEPARATOR = "\\"
  else
    SEPARATOR = "/"
  end
  
  def self.new(path, mode)
    return open_with_mode(path, mode)
  end
  
  def self.open_with_mode(path, mode)
    Ruby.primitive :io_open
  end
  
  def self.open(path, mode="r")
    raise Exception, "File.open with block is not implemented" if block_given?
    return open_with_mode(path, mode)
  end
  
  def self.raw_stat(path)
    Ruby.primitive :stat_file
  end
  
  def self.exists?(path)
    out = raw_stat(path)
    if Tuple === out
      return true
    else
      return false
    end
  end
  
  def self.file?(path)
    stat(path).kind == :file
  end

  def self.directory?(path)
    stat(path).kind == :dir
  end
  
  def self.link?(path)
    stat(path).kind == :link
  end


  def self.atime(path)
    Time.at stat(path).atime
  end

  def self.mtime(path)
    Time.at stat(path).mtime
  end

  def self.ctime(path)
    Time.at stat(path).ctime
  end

  # TODO - needs work for Win32
  def self.dirname(path)
    raise TypeError.new("can't convert nil into a pathname") if path.nil?

    slash = -1
    nonslash = 0
    before_slash = 0
    0.upto(path.length-1) do |i|
      if path[i].chr == "/" 
        slash = i
        before_slash = nonslash + 1
      else
        nonslash = i
      end
    end
    return "/" if slash > 0  and nonslash == 0
    return "." if slash == -1
    return path.slice(0, before_slash)
  end
  
  # TODO - needs work for Win32
  def self.basename(path, ext=nil)
    raise TypeError.new("can't convert nil into a pathname") if path.nil?

    slash = -1
    0.upto(path.length-1) do |i|
      if path[i].chr == "/"
        slash = i
      end
    end
    if slash == -1
      slash = 0 
    else 
      slash += 1 unless path.length == 1
    end
    bn = path.slice(slash, path.length-slash)
    if (ext)
      x = bn.length
      y = ext.length
      # TODO the to_sym works around a mysterious bug
      if (x > y && bn.slice(x-y, y).to_sym == ext.to_sym) 
        bn = bn.slice(0, x-y) 
      end
    end
    return bn 
  end
  
  def self.expand_path(path)
    path = path.to_s.strip
    dirs = path.split('/')
    if path == '' || (dirs.empty? && path[0].chr != '/')
      Dir.pwd
    else
      first = case dirs.first
      when '..' : Dir.pwd.split('/')[0...-1].join('/')
      when '~' : ENV['HOME']
      when '.' : Dir.pwd
      when '' : '/'
      else
        if dirs.first.nil?
          match = /(\/+)/.match(path) 
          prefix = match[0] if match
          ''
        else
          Dir.pwd + '/' + dirs.first
        end
      end
      dirs.shift
      paths = first.split('/')
      dirs.each do |dir|
        next if dir == '.' || dir == ''
        dir == '..' ? paths.pop : paths.push(dir)
      end
      string = paths.empty? ? '' : paths.join("/")
      !string.empty? && string[0].chr == '/' ? string : prefix || '/' +string
    end
  end

  def self.join(*args)
    args.map { |arg| arg.to_str }.join(SEPARATOR)
  end

  class Stat
    
    define_fields :inode, :mode, :kind, :owner, :group, :size, :block, :atime, :mtime, :ctime, :path
        
    def self.from_tuple(tup, path)
      obj = allocate
      obj.copy_from tup, 0
      obj.put 10, path
      return obj
    end
    
    
    def inspect
      "#<#{self.class}:0x#{object_id.to_s(16)} path=#{self.path} kind=#{self.kind}>"
    end
  end
  
  def self.stat(path)
    out = raw_stat(path)
    if !out
      raise UnableToStat.new("Unable to perform stat on '#{path}'")
    elsif out == 1
      raise NoFileError.new("'#{path}' does not exist")
    elsif out == 2
      raise PermissionError.new("Unable to access '#{path}'")
    else
      return Stat.from_tuple(out, path)
    end
  end
  
  def self.to_sexp(path, newlines)
    Ruby.primitive :file_to_sexp
  end
  
  def self.unlink(path)
    Ruby.primitive :file_unlink
  end
end

class Dir
  module Foreign
    attach_function nil, "getcwd", [:string, :int], :string
  end

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
    Foreign.getcwd(buf, buf.length)
  end

  class << self
    alias :pwd :getwd
  end
end

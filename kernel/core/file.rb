class File < IO
  class FileError < Exception; end
  class NoFileError < FileError; end
  class UnableToStat < FileError; end
  class PermissionError < FileError; end
  
  def self.new(path, mode)
    return open_with_mode(path, mode)
  end
  
  def self.open_with_mode(path, mode)
    Ruby.primitive :io_open
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
  
  class Stat
    self.instance_fields = 8
    def self.from_tuple(tup, path)
      obj = allocate
      obj.copy_from tup, 0
      obj.put 7, path
      return obj
    end
    
    index_reader :inode, 0
    index_reader :mode, 1
    index_reader :kind, 2
    index_reader :owner, 3
    index_reader :group, 4
    index_reader :size, 5
    index_reader :blocks, 6
    index_reader :path, 7
    
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
end

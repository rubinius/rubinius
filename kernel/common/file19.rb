class File
  def self.path(obj)
    return obj.to_path if obj.respond_to? :to_path

    StringValue(obj)
  end

  def size
    raise IOError, "closed stream" if closed?
    stat.size
  end

  def self.absolute_path(obj, dir = nil)
    if dir.nil?
      path(obj)
    else
      expand_path(obj, dir)
    end
  end

  ##
  # Returns true if the named files are identical.
  #
  #   open("a", "w") {}
  #   p File.identical?("a", "a")      #=> true
  #   p File.identical?("a", "./a")    #=> true
  #   File.link("a", "b")
  #   p File.identical?("a", "b")      #=> true
  #   File.symlink("a", "c")
  #   p File.identical?("a", "c")      #=> true
  #   open("d", "w") {}
  #   p File.identical?("a", "d")      #=> false
  def self.identical?(orig, copy)
    orig = Rubinius::Type.coerce_to_path(orig)
    st_o = stat(orig)
    copy = Rubinius::Type.coerce_to_path(copy)
    st_c = stat(copy)

    return false unless st_o.ino == st_c.ino
    return false unless st_o.ftype == st_c.ftype
    return false unless POSIX.access(orig, Constants::R_OK)
    return false unless POSIX.access(copy, Constants::R_OK)

    true
  end

  def self.world_readable?(path)
    path = Rubinius::Type.coerce_to_path path
    return nil unless exists? path
    mode = Stat.new(path).mode
    if (mode & Stat::S_IROTH) == Stat::S_IROTH
      tmp = mode & (Stat::S_IRUGO | Stat::S_IWUGO | Stat::S_IXUGO)
      return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
    end
    nil
  end

  def self.world_writable?(path)
    path = Rubinius::Type.coerce_to_path path
    return nil unless exists? path
    mode = Stat.new(path).mode
    if (mode & Stat::S_IWOTH) == Stat::S_IWOTH
      tmp = mode & (Stat::S_IRUGO | Stat::S_IWUGO | Stat::S_IXUGO)
      return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
    end
  end

  alias_method :to_path, :path
end

class File::Stat
  def world_readable?
    if (@stat[:st_mode] & S_IROTH) == S_IROTH
      tmp = @stat[:st_mode] & (S_IRUGO | S_IWUGO | S_IXUGO)
      return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
    end
  end

  def world_writable?
    if (@stat[:st_mode] & S_IWOTH) == S_IWOTH
      tmp = @stat[:st_mode] & (S_IRUGO | S_IWUGO | S_IXUGO)
      return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
    end
  end
end

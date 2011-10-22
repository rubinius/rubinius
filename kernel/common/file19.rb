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

# -*- encoding: us-ascii -*-

class File
  def self.path(obj)
    return obj.to_path if obj.respond_to? :to_path

    StringValue(obj)
  end

  def initialize(path_or_fd, mode=undefined, perm=undefined, options=undefined)
    if path_or_fd.kind_of? Integer
      super(path_or_fd, mode, options)
      @path = nil
    else
      path = Rubinius::Type.coerce_to_path path_or_fd

      if options.equal?(undefined)
        options = Rubinius::Type.try_convert(perm, Hash, :to_hash)
        perm = undefined if options
      end

      nmode, binary, external, internal = IO.normalize_options(mode, options)
      nmode ||= "r"

      perm = 0666 if perm.equal? undefined

      fd = IO.sysopen(path, nmode, perm)
      if fd < 0
        begin
          Errno.handle path
        rescue Errno::EMFILE
          # true means force to run, don't ignore it.
          GC.run(true)

          fd = IO.sysopen(path, nmode, perm)
          Errno.handle if fd < 0
        end
      end

      @path = path
      super(fd, mode, options)
    end
  end

  private :initialize

  def size
    raise IOError, "closed stream" if closed?
    stat.size
  end

  def self.absolute_path(obj, dir = nil)
    obj = path(obj) 
    if obj[0] == "~"
      File.join Dir.getwd, dir.to_s, obj
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

  def self.realpath(path, basedir = nil)
    path = expand_path(path, basedir || Dir.pwd)
    real = ''
    symlinks = {}

    while !path.empty?
      pos = path.index(SEPARATOR, 1)

      if pos
        name = path[0...pos]
        path = path[pos..-1]
      else
        name = path
        path = ''
      end

      real = join(real, name)
      if symlink?(real)
        raise Errno::ELOOP if symlinks[real]
        symlinks[real] = true
        if path.empty?
          path = expand_path(readlink(real))
        else
          path = expand_path(join(readlink(real), path))
        end
        real = ''
      end
    end

    real
  end
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

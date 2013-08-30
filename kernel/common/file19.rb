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

      # TODO: fix normalize_options
      case mode
      when String, Fixnum
        # do nothing
      when nil, undefined
        mode = "r"
      when Hash
        options = mode
        mode = nil
      else
        options = Rubinius::Type.coerce_to mode, Hash, :to_hash
        mode = nil
      end

      if undefined.equal?(options)
        options = Rubinius::Type.try_convert(perm, Hash, :to_hash)
        perm = undefined if options
      end

      nmode, binary, external, internal = IO.normalize_options(mode, options)
      nmode ||= "r"

      perm = 0666 if undefined.equal? perm

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
          path = expand_path(readlink(real), dirname(real))
        else
          path = expand_path(join(readlink(real), path), dirname(real))
        end
        real = ''
      end
    end

    unless exists? real
      raise Errno::ENOENT, real
    end

    real
  end

  def self.realdirpath(path, basedir = nil)
    dirname(realpath(path, basedir))
  end

  def self.expand_path(path, dir=nil)
    path = Rubinius::Type.coerce_to_path(path)
    first = path[0]
    if first == ?~
      case path[1]
      when ?/
        unless home = ENV["HOME"]
          raise ArgumentError, "couldn't find HOME environment variable when expanding '~'"
        end

        path = ENV["HOME"] + path.byteslice(1, path.bytesize - 1)
      when nil
        unless home = ENV["HOME"]
          raise ArgumentError, "couldn't find HOME environment variable when expanding '~'"
        end

        if home.empty?
          raise ArgumentError, "HOME environment variable is empty expanding '~'"
        end

        return home.dup
      else
        unless length = path.find_string("/", 1)
          length = path.bytesize
        end

        name = path.byteslice 1, length - 1
        unless dir = Rubinius.get_user_home(name)
          raise ArgumentError, "user #{name} does not exist"
        end

        path = dir + path.byteslice(length, path.bytesize - length)
      end
    elsif first != ?/
      if dir
        dir = expand_path dir
      else
        dir = PrivateDir.pwd
      end

      path = "#{dir}/#{path}"
    end

    items = []
    start = 0
    size = path.bytesize

    while index = path.find_string("/", start) or (start < size and index = size)
      length = index - start

      if length > 0
        item = path.byteslice start, length

        if item == ".."
          items.pop
        elsif item != "."
          items << item
        end
      end

      start = index + 1
    end

    if items.empty?
      str = "/"
    else
      str = ""
      items.each { |x| str.append "/#{x}" }
    end

    return Rubinius::Type.external_string(str)
  end


end

class File::Stat
  def world_readable?
    if mode & S_IROTH == S_IROTH
      tmp = mode & (S_IRUGO | S_IWUGO | S_IXUGO)
      return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
    end
  end

  def world_writable?
    if mode & S_IWOTH == S_IWOTH
      tmp = mode & (S_IRUGO | S_IWUGO | S_IXUGO)
      return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
    end
  end
end

module File::Constants
  if Rubinius.windows?
    NULL = 'NUL'
  else
    NULL = '/dev/null'
  end
end

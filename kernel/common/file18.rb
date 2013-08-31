class File
  def initialize(path_or_fd, mode=nil, perm=0666)
    if path_or_fd.kind_of? Integer
      super(path_or_fd, mode)
      @path = nil
    else
      path = Rubinius::Type.coerce_to_path path_or_fd

      mode ||= "r"
      fd = IO.sysopen(path, mode, perm)
      if fd < 0
        begin
          Errno.handle path
        rescue Errno::EMFILE
          # true means force to run, don't ignore it.
          GC.run(true)

          fd = IO.sysopen(path, mode, perm)
          Errno.handle if fd < 0
        end
      end

      @path = path
      super(fd)
    end
  end

  private :initialize

  ##
  # Converts a pathname to an absolute pathname. Relative
  # paths are referenced from the current working directory
  # of the process unless dir_string is given, in which case
  # it will be used as the starting point. The given pathname
  # may start with a ``~’’, which expands to the process owner‘s
  # home directory (the environment variable HOME must be set
  # correctly). "~user" expands to the named user‘s home directory.
  #
  #  File.expand_path("~oracle/bin")           #=> "/home/oracle/bin"
  #  File.expand_path("../../bin", "/tmp/x")   #=> "/bin"
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

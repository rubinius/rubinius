class File
  def initialize(path_or_fd, mode = "r", perm = 0666)
    if path_or_fd.kind_of? Integer
      super(path_or_fd, mode)
      @path = nil
    else
      path = Rubinius::Type.coerce_to_path path_or_fd

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
  # Equivalent to File::chmod, but does not follow symbolic
  # links (so it will change the permissions associated with
  # the link, not the file referenced by the link).
  # Often not available.
  def self.lchmod(mode, *paths)
    mode = Rubinius::Type.coerce_to(mode, Integer, :to_int)

    paths.each do |path|
      POSIX.lchmod Rubinius::Type.coerce_to_path(path), mode
    end

    paths.size
  end

end

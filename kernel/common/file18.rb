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

  def self.expand_path_buffer(path)
    ""
  end
  private_class_method :expand_path_buffer
end

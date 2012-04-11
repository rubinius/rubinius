module Rubinius
  module Spawn
    # Turns the various varargs incantations supported by Process.spawn into a
    # simple [env, prog, argv, options] tuple.
    #
    # The following method signature is supported:
    #   Process.spawn([env], command, ..., [options])
    #
    # The env and options hashes are optional. The command may be a variable
    # number of strings or an Array full of strings that make up the new process's
    # argv.
    #
    # Returns an [env, prog, argv, options] tuple. All elements are guaranteed to be
    # non-nil. When no env or options are given, empty hashes are returned.
    def self.extract_arguments(*args)
      if options = Rubinius::Type.try_convert(args.last, Hash, :to_hash)
        args.pop
        options = options.dup
      else
        options = {}
      end
      flatten_options!(options)
      normalize_redirect_file_options!(options)

      if env = Rubinius::Type.try_convert(args.first, Hash, :to_hash)
        args.shift
      else
        env = {}
      end

      # remaining arguments are the argv supporting a number of variations.
      prog, argv = adjust_argv(*args)

      [env, prog, argv, options]
    end

    # Convert { [fd1, fd2, ...] => (:close|fd) } options to individual keys,
    # like: { fd1 => :close, fd2 => :close }.
    #
    # options - The options hash. This is modified in place.
    #
    # Returns the modified options hash.
    def self.flatten_options!(options)
      options.to_a.each do |key, value|
        if key.respond_to?(:to_ary)
          key.to_ary.each { |fd| options[fd] = value }
          options.delete(key)
        end
      end
    end

    # Mapping of string open modes to integer oflag versions.
    OFLAGS = {
      "r"  => File::RDONLY,
      "r+" => File::RDWR   | File::CREAT,
      "w"  => File::WRONLY | File::CREAT  | File::TRUNC,
      "w+" => File::RDWR   | File::CREAT  | File::TRUNC,
      "a"  => File::WRONLY | File::APPEND | File::CREAT,
      "a+" => File::RDWR   | File::APPEND | File::CREAT
    }

    # Convert variations of redirecting to a file to a standard tuple.
    #
    # :in   => '/some/file'   => ['/some/file', 'r', 0644]
    # :out  => '/some/file'   => ['/some/file', 'w', 0644]
    # :err  => '/some/file'   => ['/some/file', 'w', 0644]
    # STDIN => '/some/file'   => ['/some/file', 'r', 0644]
    #
    # Returns the modified options hash.
    def self.normalize_redirect_file_options!(options)
      options.to_a.each do |key, value|
        next if !fd?(key)

        # convert string and short array values to
        if value.respond_to?(:to_str)
          value = default_file_reopen_info(key, value)
        elsif value.respond_to?(:to_ary) && value.size < 3
          defaults = default_file_reopen_info(key, value[0])
          value += defaults[value.size..-1]
        else
          value = nil
        end

        # replace string open mode flag maybe and replace original value
        if value
          value[1] = OFLAGS[value[1]] if value[1].respond_to?(:to_str)
          options[key] = value
        end
      end
    end

    # The default [file, flags, mode] tuple for a given fd and filename. The
    # default flags vary based on the what fd is being redirected. stdout and
    # stderr default to write, while stdin and all other fds default to read.
    #
    # fd   - The file descriptor that is being redirected. This may be an IO
    #        object, integer fd number, or :in, :out, :err for one of the standard
    #        streams.
    # file - The string path to the file that fd should be redirected to.
    #
    # Returns a [file, flags, mode] tuple.
    def self.default_file_reopen_info(fd, file)
      case fd
      when :in, STDIN, $stdin, 0
        [file, "r", 0644]
      when :out, STDOUT, $stdout, 1
        [file, "w", 0644]
      when :err, STDERR, $stderr, 2
        [file, "w", 0644]
      else
        [file, "r", 0644]
      end
    end

    # Determine whether object is fd-like.
    #
    # Returns true if object is an instance of IO, Fixnum >= 0, or one of the
    # the symbolic names :in, :out, or :err.
    def self.fd?(object)
      case object
      when Fixnum
        object >= 0
      when :in, :out, :err, STDIN, STDOUT, STDERR, $stdin, $stdout, $stderr, IO
        true
      else
        object.respond_to?(:to_io) && !object.to_io.nil?
      end
    end

    # Convert a fd identifier to an IO object.
    #
    # Returns nil or an instance of IO.
    def self.fd_to_io(object)
      case object
      when STDIN, STDOUT, STDERR, $stdin, $stdout, $stderr
        object
      when :in, 0
        STDIN
      when :out, 1
        STDOUT
      when :err, 2
        STDERR
      when Fixnum
        object >= 0 ? IO.for_fd(object) : nil
      when IO
        object
      else
        object.respond_to?(:to_io) ? object.to_io : nil
      end
    end

    # Converts the various supported command argument variations into a
    # standard argv suitable for use with exec. This includes detecting commands
    # to be run through the shell.
    #
    # The args array may follow any of these variations:
    #
    # 'true'                     => ['true', ['true']]
    # 'echo', 'hello', 'world'   => ['echo', ['echo', 'hello', 'world']]
    # 'echo *'                   => ['/bin/sh', ['sh', '-c', 'echo *']]
    # ['echo', 'fuuu'], 'hello'  => ['echo', ['fuuu', 'hello']]
    #
    # Returns [prog, argv]
    def self.adjust_argv(first, *args)
      if args.empty? and cmd = Rubinius::Type.try_convert(first, String, :to_str)
        if cmd.empty?
          raise Errno::ENOENT
        elsif /([*?{}\[\]<>()~&|$;'`"\n])/o.match(cmd)
          return ["/bin/sh", ["sh", "-c", cmd]]
        else
          args = cmd.split(' ')
          return [args.first, args]
        end
      elsif cmd = Rubinius::Type.try_convert(first, Array, :to_ary)
        raise ArgumentError, "wrong first argument" unless cmd.size == 2
        prog = StringValue(cmd[0])
        name = StringValue(cmd[1])
      else
        name = prog = StringValue(first)
      end

      argv = [name]
      args.each do |arg|
        argv << StringValue(arg)
      end

      [prog, argv]
    end

    def self.exec(env, prog, argv, options)
      # handle FD => {FD, :close, [file,mode,perms]} options
      options.each do |key, val|
        if fd?(key)
          key = fd_to_io(key)

          if fd?(val)
            val = fd_to_io(val)
            key.reopen(val)
          elsif val == :close
            if key.respond_to?(:close_on_exec=)
              key.close_on_exec = true
            else
              key.close
            end
          elsif val.is_a?(Array)
            file, mode_string, perms = *val
            key.reopen(File.open(file, mode_string, perms))
          end
        end
      end

      env.each { |key, value| ENV[key] = value }

      if options[:unsetenv_others]
        ENV.keep_if { |key, _| env.key?(key) }
      end

      if chdir = options[:chdir]
        Dir.chdir(chdir)
      end

      # { :pgroup => pgid } options
      pgroup = options[:pgroup]
      pgroup = 0 if pgroup == true
      Process.setpgid(0, pgroup) if pgroup

      Process.perform_exec prog, argv
    end
  end
end

module Process
  def self.daemon(stay_in_dir=false, keep_stdio_open=false)
    # Do not run at_exit handlers in the parent
    exit!(0) if fork

    Process.setsid

    exit!(0) if fork

    Dir.chdir("/") unless stay_in_dir

    unless keep_stdio_open
      io = File.open "/dev/null", File::RDWR, 0
      $stdin.reopen io
      $stdout.reopen io
      $stderr.reopen io
    end

    return 0
  end

  def self.exec(*args)
    env, prog, argv, options = Rubinius::Spawn.extract_arguments(*args)
    Rubinius::Spawn.exec(env, prog, argv, options)
  end

  def self.spawn(*args)
    env, prog, argv, options = Rubinius::Spawn.extract_arguments(*args)

    IO.pipe do |read, write|
      pid = Process.fork do
        read.close
        write.close_on_exec = true

        begin
          Rubinius::Spawn.exec(env, prog, argv, options)
        rescue => e
          write.write Marshal.dump(e)
          exit! 1
        end
      end

      write.close
      raise Marshal.load(read) unless read.eof?

      pid
    end
  end
end

module Kernel
  def system(*args)
    begin
      pid = Process.spawn(*args)
    rescue SystemCallError
      nil
    else
      Process.waitpid(pid)
      $?.exitstatus == 0
    end
  end
  module_function :system

  def spawn(*args)
    Process.spawn(*args)
  end
  module_function :spawn
end

module Rubinius
  module Spawn
    # Turns the various varargs incantations supported by Process.spawn into a
    # [env, prog, argv, redirects, options] tuple.
    #
    # The following method signature is supported:
    #   Process.spawn([env], command, ..., [options])
    #
    # The env and options hashes are optional. The command may be a variable
    # number of strings or an Array full of strings that make up the new process's
    # argv.
    #
    # Returns an [env, prog, argv, redirects, options] tuple. All elements are
    # guaranteed to be non-nil. When no env or options are given, empty hashes
    # are returned.
    def self.extract_arguments(*args)
      if options = Rubinius::Type.try_convert(args.last, Hash, :to_hash)
        args.pop
      else
        options = {}
      end

      if env = Rubinius::Type.try_convert(args.first, Hash, :to_hash)
        args.shift
      else
        env = {}
      end

      env         = adjust_env(env)
      prog, argv  = adjust_argv(*args)
      redir, opts = adjust_options(options)

      [env, prog, argv, redir, opts]
    end

    def self.adjust_env(env)
      adjusted = {}

      env.each do |key, value|
        key   = Rubinius::Type.check_null_safe(StringValue(key))
        value = Rubinius::Type.check_null_safe(StringValue(value)) unless value.nil?

        if key.include?("=")
          raise ArgumentError, "environment name contains a equal : #{key}"
        end

        adjusted[key] = value
      end

      adjusted
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

    def self.adjust_options(options)
      redirects = {}
      others = {}

      options.each do |key, value|
        case key
          when Array
            key.each do |fd|
              redirects[fd] = adjust_redirect_value(fd, value)
            end
          when :in, :out, :err, Fixnum, IO
            redirects[key] = adjust_redirect_value(key, value)
          when :pgroup
            if value == true
              value = 0
            elsif value
              value = Rubinius::Type.coerce_to value, Integer, :to_int
              raise ArgumentError, "negative process group ID : #{value}" if value < 0
            end
            others[key] = value
          when :chdir
            others[key] = Rubinius::Type.coerce_to_path(value)
          when :unsetenv_others, :umask, :close_others
            others[key] = value
          when Symbol
            raise ArgumentError, "wrong exec option symbol : #{key.inspect}"
          else
            raise ArgumentError, "wrong exec option"
        end
      end

      [redirects, others]
    end

    # Convert variations of redirecting to a file to a standard tuple.
    #
    # :in   => '/some/file'   => ['/some/file', 'r', 0644]
    # :out  => '/some/file'   => ['/some/file', 'w', 0644]
    # :err  => '/some/file'   => ['/some/file', 'w', 0644]
    # STDIN => '/some/file'   => ['/some/file', 'r', 0644]
    #
    # Returns the modified value.
    def self.adjust_redirect_value(key, value)
      case value
        when :in, :out, :err, :close, IO, Fixnum
          value
        when Array
          if value.size < 3
            defaults = default_file_reopen_info(key, value[0])
            value += defaults[value.size..-1]
          end

          if value[1].respond_to?(:to_str)
            value = value.dup
            value[1] = OFLAGS[value[1]]
          end

          value
        when String
          default_file_reopen_info(key, value)
        when Symbol
          raise ArgumentError, "wrong exec redirect symbol : #{value.inspect}"
        else
          raise ArgumentError, "wrong exec redirect action"
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
        [file, OFLAGS["r"], 0644]
      when :out, STDOUT, $stdout, 1
        [file, OFLAGS["w"], 0644]
      when :err, STDERR, $stderr, 2
        [file, OFLAGS["w"], 0644]
      else
        [file, OFLAGS["r"], 0644]
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
        else
          return [cmd, []]
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

    def self.setup_redirects(redirects)
      redirects.each do |key, val|
        key = fd_to_io(key)

        if val == :close
          key.close_on_exec = true
        elsif val.is_a?(Array)
          file, mode_string, perms = *val
          key.reopen(File.open(file, mode_string, perms))
        else
          val = fd_to_io(val)
          val.close_on_exec = false
          val.autoclose = false
          key.reopen(val)
        end
      end
    end

    def self.setup_options(options)
      ENV.clear if options[:unsetenv_others]

      if chdir = options[:chdir]
        Dir.chdir(chdir)
      end

      # { :pgroup => pgid } options
      pgroup = options[:pgroup]
      pgroup = 0 if pgroup == true
      Process.setpgid(0, pgroup) if pgroup

      if umask = options[:umask]
        File.umask(umask)
      end

      unless options[:close_others] == false
        3.upto(IO.max_open_fd).each do |fd|
          begin
            IO.for_fd(fd, :autoclose => false).close_on_exec = true
          rescue Errno::EBADF
          end
        end
      end
    end

    def self.exec(env, prog, argv, redirects, options)
      setup_options(options)
      setup_redirects(redirects)
      ENV.update(env)
      Process.perform_exec prog, argv
    end
  end
end

module Process
  Rubinius::Globals.read_only :$?
  Rubinius::Globals.set_hook(:$?) { Thread.current[:$?] }

  def self.set_status_global(status)
    Thread.current[:$?] = status
  end

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
    env, prog, argv, redirects, options = Rubinius::Spawn.extract_arguments(*args)
    Rubinius::Spawn.exec(env, prog, argv, redirects, options)
  end

  def self.spawn(*args)
    env, prog, argv, redirects, options = Rubinius::Spawn.extract_arguments(*args)

    IO.pipe do |read, write|
      pid = Process.fork do
        read.close_on_exec = true
        write.close_on_exec = true

        begin
          Rubinius::Spawn.exec(env, prog, argv, redirects, options)
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

  # TODO: Should an error be raised on ECHILD? --rue
  #
  # TODO: This operates on the assumption that waiting on
  #       the event consumes very little resources. If this
  #       is not the case, the check should be made WNOHANG
  #       and called periodically.
  #
  def self.detach(pid)
    raise ArgumentError, "Only positive pids may be detached" unless pid > 0

    thread = Thread.new { Process.wait pid; $? }
    thread[:pid] = pid
    def thread.pid; self[:pid] end

    thread
  end

  def self.coerce_rlimit_resource(resource)
    case resource
    when Integer
      return resource
    when Symbol, String
      # do nothing
    else
      unless r = Rubinius::Type.check_convert_type(resource, String, :to_str)
        return Rubinius::Type.coerce_to resource, Integer, :to_int
      end

      resource = r
    end

    constant = "RLIMIT_#{resource}"
    unless const_defined? constant
      raise ArgumentError, "invalid resource name: #{constant}"
    end
    const_get constant
  end
  private_class_method :coerce_rlimit_resource
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

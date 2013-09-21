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
  module Constants
    EXIT_SUCCESS = Rubinius::Config['rbx.platform.process.EXIT_SUCCESS'] || 0
    EXIT_FAILURE = Rubinius::Config['rbx.platform.process.EXIT_FAILURE'] || 1

    PRIO_PGRP    = Rubinius::Config['rbx.platform.process.PRIO_PGRP']
    PRIO_PROCESS = Rubinius::Config['rbx.platform.process.PRIO_PROCESS']
    PRIO_USER    = Rubinius::Config['rbx.platform.process.PRIO_USER']

    RLIM_INFINITY  = Rubinius::Config['rbx.platform.process.RLIM_INFINITY']
    RLIM_SAVED_MAX = Rubinius::Config['rbx.platform.process.RLIM_SAVED_MAX']
    RLIM_SAVED_CUR = Rubinius::Config['rbx.platform.process.RLIM_SAVED_CUR']

    RLIMIT_AS      = Rubinius::Config['rbx.platform.process.RLIMIT_AS']
    RLIMIT_CORE    = Rubinius::Config['rbx.platform.process.RLIMIT_CORE']
    RLIMIT_CPU     = Rubinius::Config['rbx.platform.process.RLIMIT_CPU']
    RLIMIT_DATA    = Rubinius::Config['rbx.platform.process.RLIMIT_DATA']
    RLIMIT_FSIZE   = Rubinius::Config['rbx.platform.process.RLIMIT_FSIZE']
    RLIMIT_MEMLOCK = Rubinius::Config['rbx.platform.process.RLIMIT_MEMLOCK']
    RLIMIT_NOFILE  = Rubinius::Config['rbx.platform.process.RLIMIT_NOFILE']
    RLIMIT_NPROC   = Rubinius::Config['rbx.platform.process.RLIMIT_NPROC']
    RLIMIT_RSS     = Rubinius::Config['rbx.platform.process.RLIMIT_RSS']
    RLIMIT_SBSIZE  = Rubinius::Config['rbx.platform.process.RLIMIT_SBSIZE']
    RLIMIT_STACK   = Rubinius::Config['rbx.platform.process.RLIMIT_STACK']

    RLIMIT_RTPRIO     = Rubinius::Config['rbx.platform.process.RLIMIT_RTPRIO']
    RLIMIT_RTTIME     = Rubinius::Config['rbx.platform.process.RLIMIT_RTTIME']
    RLIMIT_SIGPENDING = Rubinius::Config['rbx.platform.process.RLIMIT_SIGPENDING']
    RLIMIT_MSGQUEUE   = Rubinius::Config['rbx.platform.process.RLIMIT_MSGQUEUE']
    RLIMIT_NICE       = Rubinius::Config['rbx.platform.process.RLIMIT_NICE']

    WNOHANG = 1
    WUNTRACED = 2
  end
  include Constants

  FFI = Rubinius::FFI

  class Rlimit < FFI::Struct
    config "rbx.platform.rlimit", :rlim_cur, :rlim_max
  end

  def self.setrlimit(resource, cur_limit, max_limit=undefined)
    resource =  coerce_rlimit_resource(resource)
    cur_limit = Rubinius::Type.coerce_to cur_limit, Integer, :to_int

    unless undefined.equal? max_limit
      max_limit = Rubinius::Type.coerce_to max_limit, Integer, :to_int
    end

    rlimit = Rlimit.new
    rlimit[:rlim_cur] = cur_limit
    rlimit[:rlim_max] = undefined.equal?(max_limit) ? cur_limit : max_limit

    ret = FFI::Platform::POSIX.setrlimit(resource, rlimit.pointer)
    Errno.handle if ret == -1
    nil
  end

  def self.getrlimit(resource)
    resource = coerce_rlimit_resource(resource)

    lim_max = []
    rlimit = Rlimit.new
    ret = FFI::Platform::POSIX.getrlimit(resource, rlimit.pointer)
    Errno.handle if ret == -1

    [rlimit[:rlim_cur], rlimit[:rlim_max]]
  end

  def self.setsid
    pgid = FFI::Platform::POSIX.setsid
    Errno.handle if pgid == -1
    pgid
  end

  def self.fork
    pid = perform_fork
    pid = nil if pid == 0
    if block_given? and pid.nil?
      begin
        yield nil
        status = 0
      rescue SystemExit => e
        status = e.status
      rescue Exception => e
        e.render "An exception occurred in a forked block"
        status = 1
      end

      until Rubinius::AtExit.empty?
        begin
          Rubinius::AtExit.shift.call
        rescue SystemExit => e
          status = e.status
        end
      end

      ObjectSpace.run_finalizers

      # Do not use Kernel.exit. This raises a SystemExit exception, which
      # will run ensure blocks. This is not what MRI does and causes bugs
      # in programs. See issue http://github.com/rubinius/rubinius/issues#issue/289 for
      # an example

      Kernel.exit! status
    end
    pid
  end

  def self.times
    Struct::Tms.new(*cpu_times)
  end

  def self.kill(signal, *pids)
    raise ArgumentError, "PID argument required" if pids.length == 0

    use_process_group = false
    signal = signal.to_s if signal.kind_of?(Symbol)

    if signal.kind_of?(String)
      if signal[0] == ?-
        signal = signal[1..-1]
        use_process_group = true
      end

      if signal[0..2] == "SIG"
        signal = signal[3..-1]
      end

      signal = Signal::Names[signal]
    end

    raise ArgumentError unless signal.kind_of? Fixnum

    if signal < 0
      signal = -signal
      use_process_group = true
    end

    pids.each do |pid|
      pid = Rubinius::Type.coerce_to_pid pid

      pid = -pid if use_process_group
      result = FFI::Platform::POSIX.kill(pid, signal)

      Errno.handle if result == -1
    end

    return pids.length
  end

  def self.abort(msg=nil)
    if msg
      msg = StringValue(msg)
      $stderr.puts(msg)
    end
    raise SystemExit.new(1, msg)
  end

  def self.getpgid(pid)
    pid = Rubinius::Type.coerce_to pid, Integer, :to_int

    ret = FFI::Platform::POSIX.getpgid(pid)
    Errno.handle if ret == -1
    ret
  end

  def self.setpgid(pid, int)
    pid = Rubinius::Type.coerce_to pid, Integer, :to_int
    int = Rubinius::Type.coerce_to int, Integer, :to_int

    ret = FFI::Platform::POSIX.setpgid(pid, int)
    Errno.handle if ret == -1
    ret
  end

  @maxgroups = 32
  class << self
    attr_reader :maxgroups
    def maxgroups=(m)
      @maxgroups = m
    end
  end

  def self.setpgrp
    setpgid(0, 0)
  end
  def self.getpgrp
    ret = FFI::Platform::POSIX.getpgrp
    Errno.handle if ret == -1
    ret
  end

  def self.pid
    ret = FFI::Platform::POSIX.getpid
    Errno.handle if ret == -1
    ret
  end

  def self.ppid
    ret = FFI::Platform::POSIX.getppid
    Errno.handle if ret == -1
    ret
  end

  def self.uid=(uid)
    # the 4 rescue clauses below are needed
    # until respond_to? can be used to query the implementation of methods attached via FFI
    # atm respond_to returns true if a method is attached but not implemented on the platform
    uid = Rubinius::Type.coerce_to uid, Integer, :to_int
    begin
      ret = FFI::Platform::POSIX.setresuid(uid, -1, -1)
    rescue NotImplementedError
      begin
        ret = FFI::Platform::POSIX.setreuid(uid, -1)
      rescue NotImplementedError
        begin
          ret = FFI::Platform::POSIX.setruid(uid)
        rescue NotImplementedError
          if Process.euid == uid
            ret = FFI::Platform::POSIX.setuid(uid)
          else
            raise NotImplementedError
          end
        end
      end
    end

    Errno.handle if ret == -1

    uid
  end

  def self.gid=(gid)
    gid = Rubinius::Type.coerce_to gid, Integer, :to_int
    Process::Sys.setgid gid
  end

  def self.euid=(uid)
    # the 4 rescue clauses below are needed
    # until respond_to? can be used to query the implementation of methods attached via FFI
    # atm respond_to returns true if a method is attached but not implemented on the platform
    uid = Rubinius::Type.coerce_to uid, Integer, :to_int
    begin
      ret = FFI::Platform::POSIX.setresuid(-1, uid, -1)
    rescue NotImplementedError
      begin
        ret = FFI::Platform::POSIX.setreuid(-1, uid)
      rescue NotImplementedError
        begin
          ret = FFI::Platform::POSIX.seteuid(uid)
        rescue NotImplementedError
          if Process.uid == uid
            ret = FFI::Platform::POSIX.setuid(uid)
          else
            raise NotImplementedError
          end
        end
      end
    end

    Errno.handle if ret == -1

    uid
  end

  def self.egid=(gid)
    gid = Rubinius::Type.coerce_to gid, Integer, :to_int
    Process::Sys.setegid gid
  end

  def self.uid
    ret = FFI::Platform::POSIX.getuid
    Errno.handle if ret == -1
    ret
  end

  def self.gid
    ret = FFI::Platform::POSIX.getgid
    Errno.handle if ret == -1
    ret
  end

  def self.euid
    ret = FFI::Platform::POSIX.geteuid
    Errno.handle if ret == -1
    ret
  end

  def self.egid
    ret = FFI::Platform::POSIX.getegid
    Errno.handle if ret == -1
    ret
  end

  def self.getpriority(kind, id)
    kind = Rubinius::Type.coerce_to kind, Integer, :to_int
    id =   Rubinius::Type.coerce_to id, Integer, :to_int

    FFI::Platform::POSIX.errno = 0
    ret = FFI::Platform::POSIX.getpriority(kind, id)
    Errno.handle
    ret
  end

  def self.setpriority(kind, id, priority)
    kind = Rubinius::Type.coerce_to kind, Integer, :to_int
    id =   Rubinius::Type.coerce_to id, Integer, :to_int
    priority = Rubinius::Type.coerce_to priority, Integer, :to_int

    ret = FFI::Platform::POSIX.setpriority(kind, id, priority)
    Errno.handle if ret == -1
    ret
  end

  def self.groups
    g = []
    FFI::MemoryPointer.new(:int, @maxgroups) { |p|
      num_groups = FFI::Platform::POSIX.getgroups(@maxgroups, p)
      Errno.handle if num_groups == -1
      g = p.read_array_of_int(num_groups)
    }
    g
  end

  def self.groups=(g)
    @maxgroups = g.length if g.length > @maxgroups
    FFI::MemoryPointer.new(:int, @maxgroups) { |p|
      p.write_array_of_int(g)
      Errno.handle if FFI::Platform::POSIX.setgroups(g.length, p) == -1
    }
    g
  end

  def self.initgroups(username, gid)
    username = StringValue(username)
    gid = Rubinius::Type.coerce_to gid, Integer, :to_int

    if FFI::Platform::POSIX.initgroups(username, gid) == -1
      Errno.handle
    end

    Process.groups
  end

  #
  # Wait for the given process to exit.
  #
  # The pid may be the specific pid of some previously forked
  # process, or -1 to indicate to watch for *any* child process
  # exiting. Other options, such as process groups, may be available
  # depending on the system.
  #
  # With no arguments the default is to block waiting for any
  # child processes (pid -1.)
  #
  # The flag may be Process::WNOHANG, which indicates that
  # the child should only be quickly checked. If it has not
  # exited yet, nil is returned immediately instead.
  #
  # The return value is the exited pid or nil if Process::WNOHANG
  # was used and the child had not yet exited.
  #
  # If the pid has exited, the global $? is set to a Process::Status
  # object representing the exit status (and possibly other info) of
  # the child.
  #
  # If there exists no such pid (e.g. never forked or already
  # waited for), or no children at all, Errno::ECHILD is raised.
  #
  # TODO: Support other options such as WUNTRACED? --rue
  #
  def self.wait2(input_pid=-1, flags=nil)
    input_pid = Rubinius::Type.coerce_to input_pid, Integer, :to_int

    if flags and (flags & WNOHANG) == WNOHANG
      value = wait_pid_prim input_pid, true
      return if value.nil?
    else
      value = wait_pid_prim input_pid, false
    end

    if value == false
      raise Errno::ECHILD, "No child process: #{input_pid}"
    end

    # wait_pid_prim returns a tuple when wait needs to communicate
    # the pid that was actually detected as stopped (since wait
    # can wait for all child pids, groups, etc)
    status, termsig, stopsig, pid = value

    status = Process::Status.new(pid, status, termsig, stopsig)
    set_status_global status
    [pid, status]
  end

  #
  # Wait for all child processes.
  #
  # Blocks until all child processes have exited, and returns
  # an Array of [pid, Process::Status] results, one for each
  # child.
  #
  # Be mindful of the effects of creating new processes while
  # .waitall has been called (usually in a different thread.)
  # The .waitall call does not in any way check that it is only
  # waiting for children that existed at the time it was called.
  #
  def self.waitall
    statuses = []

    begin
      while true
        statuses << Process.wait2
      end
    rescue Errno::ECHILD
    end

    statuses
  end

  def self.wait(pid=-1, flags=nil)
    pid, status = Process.wait2(pid, flags)
    return pid
  end

  class << self
    alias_method :waitpid, :wait
    alias_method :waitpid2, :wait2
  end

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

  #--
  # TODO: Most of the fields aren't implemented yet.
  # TODO: Also, these objects should only need to be constructed by
  # Process.wait and family.
  #++

  class Status

    attr_reader :exitstatus
    attr_reader :termsig
    attr_reader :stopsig

    def initialize(pid, exitstatus, termsig=nil, stopsig=nil)
      @pid = pid
      @exitstatus = exitstatus
      @termsig = termsig
      @stopsig = stopsig
    end

    def to_i
      @exitstatus
    end

    def to_s
      @exitstatus.to_s
    end

    def &(num)
      @exitstatus & num
    end

    def ==(other)
      other = other.to_i if other.kind_of? Process::Status
      @exitstatus == other
    end

    def >>(num)
      @exitstatus >> num
    end

    def coredump?
      false
    end

    def exited?
      @exitstatus != nil
    end

    def pid
      @pid
    end

    def signaled?
      @termsig != nil
    end

    def stopped?
      @stopsig != nil
    end

    def success?
      if exited?
        @exitstatus == 0
      else
        nil
      end
    end
  end

  module Sys
    class << self
      def getegid
        ret = FFI::Platform::POSIX.getegid
        Errno.handle if ret == -1
        ret
      end

      def geteuid
        ret = FFI::Platform::POSIX.geteuid
        Errno.handle if ret == -1
        ret
      end

      def getgid
        ret = FFI::Platform::POSIX.getgid
        Errno.handle if ret == -1
        ret
      end

      def getuid
        ret = FFI::Platform::POSIX.getuid
        Errno.handle if ret == -1
        ret
      end

      def issetugid
        raise "not implemented"
      end

      def setgid(gid)
        gid = Rubinius::Type.coerce_to gid, Integer, :to_int

        ret = FFI::Platform::POSIX.setgid gid
        Errno.handle if ret == -1
        nil
      end

      def setuid(uid)
        uid = Rubinius::Type.coerce_to uid, Integer, :to_int

        ret = FFI::Platform::POSIX.setuid uid
        Errno.handle if ret == -1
        nil
      end

      def setegid(egid)
        egid = Rubinius::Type.coerce_to egid, Integer, :to_int

        ret = FFI::Platform::POSIX.setegid egid
        Errno.handle if ret == -1
        nil
      end

      def seteuid(euid)
        euid = Rubinius::Type.coerce_to euid, Integer, :to_int

        ret = FFI::Platform::POSIX.seteuid euid
        Errno.handle if ret == -1
        nil
      end

      def setrgid(rgid)
        setregid(rgid, -1)
      end

      def setruid(ruid)
        setreuid(ruid, -1)
      end

      def setregid(rid, eid)
        rid = Rubinius::Type.coerce_to rid, Integer, :to_int
        eid = Rubinius::Type.coerce_to eid, Integer, :to_int

        ret = FFI::Platform::POSIX.setregid rid, eid
        Errno.handle if ret == -1
        nil
      end

      def setreuid(rid, eid)
        rid = Rubinius::Type.coerce_to rid, Integer, :to_int
        eid = Rubinius::Type.coerce_to eid, Integer, :to_int

        ret = FFI::Platform::POSIX.setreuid rid, eid
        Errno.handle if ret == -1
        nil
      end

      def setresgid(rid, eid, sid)
        rid = Rubinius::Type.coerce_to rid, Integer, :to_int
        eid = Rubinius::Type.coerce_to eid, Integer, :to_int
        sid = Rubinius::Type.coerce_to sid, Integer, :to_int

        ret = FFI::Platform::POSIX.setresgid rid, eid, sid
        Errno.handle if ret == -1
        nil
      end

      def setresuid(rid, eid, sid)
        rid = Rubinius::Type.coerce_to rid, Integer, :to_int
        eid = Rubinius::Type.coerce_to eid, Integer, :to_int
        sid = Rubinius::Type.coerce_to sid, Integer, :to_int

        ret = FFI::Platform::POSIX.setresuid rid, eid, sid
        Errno.handle if ret == -1
        nil
      end
    end
  end

  module UID
    class << self
      def change_privilege(uid)
        uid = Rubinius::Type.coerce_to uid, Integer, :to_int

        ret = FFI::Platform::POSIX.setreuid(uid, uid)
        Errno.handle if ret == -1
        uid
      end

      def eid
        ret = FFI::Platform::POSIX.geteuid
        Errno.handle if ret == -1
        ret
      end

      def eid=(uid)
        uid = Rubinius::Type.coerce_to uid, Integer, :to_int

        ret = FFI::Platform::POSIX.seteuid(uid)
        Errno.handle if ret == -1
        uid
      end
      alias_method :grant_privilege, :eid=

      def re_exchange
        real = FFI::Platform::POSIX.getuid
        Errno.handle if real == -1
        eff = FFI::Platform::POSIX.geteuid
        Errno.handle if eff == -1
        ret = FFI::Platform::POSIX.setreuid(eff, real)
        Errno.handle if ret == -1
        eff
      end

      def re_exchangeable?
        true
      end

      def rid
        ret = FFI::Platform::POSIX.getuid
        Errno.handle if ret == -1
        ret
      end

      def sid_available?
        true
      end

      def switch
        eff = re_exchange
        if block_given?
          ret = yield
          re_exchange
          return ret
        else
          return eff
        end
      end

    end
  end

  module GID
    class << self
      def change_privilege(gid)
        gid = Rubinius::Type.coerce_to gid, Integer, :to_int

        ret = FFI::Platform::POSIX.setregid(gid, gid)
        Errno.handle if ret == -1
        gid
      end

      def eid
        ret = FFI::Platform::POSIX.getegid
        Errno.handle if ret == -1
        ret
      end

      def eid=(gid)
        gid = Rubinius::Type.coerce_to gid, Integer, :to_int

        ret = FFI::Platform::POSIX.setegid(gid)
        Errno.handle if ret == -1
        gid
      end
      alias_method :grant_privilege, :eid=

      def re_exchange
        real = FFI::Platform::POSIX.getgid
        Errno.handle if real == -1
        eff = FFI::Platform::POSIX.getegid
        Errno.handle if eff == -1
        ret = FFI::Platform::POSIX.setregid(eff, real)
        Errno.handle if ret == -1
        eff
      end

      def re_exchangeable?
        true
      end

      def rid
        ret = FFI::Platform::POSIX.getgid
        Errno.handle if ret == -1
        ret
      end

      def sid_available?
        true
      end

      def switch
        eff = re_exchange
        if block_given?
          ret = yield
          re_exchange
          return ret
        else
          return eff
        end
      end

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

  def fork(&block)
    Process.fork(&block)
  end
  module_function :fork

  def `(str) #`
    str = StringValue(str) unless str.kind_of?(String)
    pid, output = Process.replace(str)

    Process.waitpid(pid)
    return Rubinius::Type.external_string(output)
  end

  module_function :` # `
end

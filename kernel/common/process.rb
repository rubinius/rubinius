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

    WNOHANG = 1
    WUNTRACED = 2
  end
  include Constants

  class Rlimit < FFI::Struct
    config "rbx.platform.rlimit", :rlim_cur, :rlim_max
  end

  def self.setrlimit(resource, cur_limit, max_limit=undefined)
    resource =  Rubinius::Type.coerce_to resource, Integer, :to_int
    cur_limit = Rubinius::Type.coerce_to cur_limit, Integer, :to_int

    unless max_limit.equal? undefined
      max_limit = Rubinius::Type.coerce_to max_limit, Integer, :to_int
    end

    rlimit = Rlimit.new
    rlimit[:rlim_cur] = cur_limit
    rlimit[:rlim_max] = max_limit.equal?(undefined) ? cur_limit : max_limit

    ret = FFI::Platform::POSIX.setrlimit(resource, rlimit.pointer)
    Errno.handle if ret == -1
    nil
  end

  def self.getrlimit(resource)
    resource =  Rubinius::Type.coerce_to resource, Integer, :to_int

    lim_max = []
    rlimit = Rlimit.new
    ret = FFI::Platform::POSIX.getrlimit(resource, rlimit.pointer)
    Errno.handle if ret == -1

    [rlimit[:rlim_cur], rlimit[:rlim_max]]
  end

  def self.setsid
    pgid = FFI::Platform::POSIX.setsid
    Errno.handle if -1 == pgid
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
        e.render "An exception occured in a forked block"
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
      # in programs. See issue http://github.com/evanphx/rubinius/issues#issue/289 for
      # an example

      Kernel.exit! status
    end
    pid
  end

  def self.times
    Struct::Tms.new(*cpu_times)
  end

  def self.kill(sig, pid)
    use_process_group = false
    sig = sig.to_s if sig.kind_of?(Symbol)

    if sig.kind_of?(String)
      if sig[0] == ?-
        sig = sig[1..-1]
        use_process_group = true
      end
      if sig[0..2] == "SIG"
        sig = sig[3..-1]
      end
      number = Signal::Names[sig]
    else
      number = sig.to_i
      if number < 0
        number = -number
        use_process_group = true
      end
    end

    raise ArgumentError unless number

    pid = Rubinius::Type.coerce_to pid, Integer, :to_int

    pid = -pid if use_process_group
    ret = FFI::Platform::POSIX.kill(pid, number)

    case ret
    when 0
      return 1
    when -1
      Errno.handle
    end
  end

  def self.abort(msg=nil)
    $stderr.puts(msg) if msg
    raise SystemExit.new(1)
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
    uid = Rubinius::Type.coerce_to uid, Integer, :to_int
    Process::Sys.setuid uid
  end

  def self.gid=(gid)
    gid = Rubinius::Type.coerce_to gid, Integer, :to_int
    Process::Sys.setgid gid
  end

  def self.euid=(uid)
    uid = Rubinius::Type.coerce_to uid, Integer, :to_int
    Process::Sys.seteuid uid
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
      Errno.handle if -1 == FFI::Platform::POSIX.setgroups(g.length, p)
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
    Rubinius::Globals.set! :$?, status
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

  #
  # Indicate disinterest in child process.
  #
  # Sets up an internal wait on the given process ID.
  # Only possibly real pids, i.e. positive numbers,
  # may be waited for.
  #
  # TODO: Should an error be raised on ECHILD? --rue
  #
  # TODO: This operates on the assumption that waiting on
  #       the event consumes very little resources. If this
  #       is not the case, the check should be made WNOHANG
  #       and called periodically.
  #
  def self.detach(pid)
    raise ArgumentError, "Only positive pids may be detached" unless pid > 0

    # The evented system does not need a loop
    Thread.new { Process.wait pid }
  end

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

      def setreuid(rid)
        rid = Rubinius::Type.coerce_to rid, Integer, :to_int

        ret = FFI::Platform::POSIX.setreuid rid
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

      def setresuid(rid, eig, sid)
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

  def fork(&block)
    Process.fork(&block)
  end
  module_function :fork

  def system(prog, *args)
    pid = Process.fork
    if pid
      Process.waitpid(pid)
      $?.exitstatus == 0
    else
      begin
        Kernel.exec(prog, *args)
      rescue Exception => e
        if $DEBUG
          e.render("Unable to execute subprogram", STDERR)
        end
        exit! 1
      end

      if $DEBUG
        STDERR.puts "Unable to execute subprogram - exec silently returned"
      end
      exit! 1
    end
  end
  module_function :system

  def exec(cmd, *args)
    if args.empty? and cmd.kind_of? String
      raise Errno::ENOENT if cmd.empty?
      if /([*?{}\[\]<>()~&|$;'`"\n\s]|[^\w])/o.match(cmd)
        Process.perform_exec "/bin/sh", ["sh", "-c", cmd]
      else
        Process.perform_exec cmd, [cmd]
      end
    else
      if cmd.kind_of? Array
        prog = cmd[0]
        name = cmd[1]
      else
        name = prog = cmd
      end

      argv = [name]
      args.each do |arg|
        argv << arg.to_s
      end

      Process.perform_exec prog, argv
    end
  end
  module_function :exec

  def `(str) #`
    str = StringValue(str) unless str.kind_of?(String)
    pid, output = Process.replace(str)

    Process.waitpid(pid)
    return output
  end

  module_function :` # `
end

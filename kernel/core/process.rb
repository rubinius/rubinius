# depends on: module.rb class.rb struct.rb

module Process
  WNOHANG = 1
  WUNTRACED = 2
  PRIO_PROCESS = 0
  PRIO_PGRP = 1
  PRIO_USER = 2
  RLIM_INFINITY = 18446744073709551615
  RLIM_SAVED_MAX = 18446744073709551615
  RLIM_SAVED_CUR = 18446744073709551615
  RLIMIT_CPU = 0
  RLIMIT_FSIZE = 1
  RLIMIT_DATA = 2
  RLIMIT_STACK = 3
  RLIMIT_CORE = 4
  RLIMIT_RSS = 5
  RLIMIT_NPROC = 6
  RLIMIT_NOFILE = 7
  RLIMIT_MEMLOCK = 8
  RLIMIT_AS = 9

  class Rlimit < FFI::Struct
    layout :rlim_cur, :ulong, 0, :rlim_max, :ulong, FFI.type_size(:ulong)
  end

  def self.setrlimit(resource, cur_limit, max_limit=Undefined)
    MemoryPointer.new(:ulong, 2) { |p|
      rlimit = Rlimit.new(p)
      rlimit[:rlim_cur] = cur_limit
      rlimit[:rlim_max] = max_limit.equal?(Undefined) ? cur_limit : max_limit
      Errno.handle if -1 == Platform::POSIX.setrlimit(resource, p)
    }
    nil
  end

  def self.getrlimit(resource)
    lim_max = []
    MemoryPointer.new(:ulong, 2) { |p|
      Errno.handle if -1 == Platform::POSIX.getrlimit(resource, p)
      rlimit = Rlimit.new(p)
      lim_max = [rlimit[:rlim_cur], rlimit[:rlim_max]]
    }
    lim_max
  end

  def self.setsid
    pgid = Platform::POSIX.setsid
    Errno.handle if -1 == pgid
    pgid
  end

  def self.fork
    pid = fork_prim
    pid = nil if pid == 0
    if block_given? and pid.nil?
      yield nil
      Kernel.exit
    end
    pid
  end

  def self.sleep(sec)
    micro_sleep(sec * 1_000_000)
  end
  
  def self.usleep(sec)
    micro_sleep(sec * 1_000)
  end
  
  def self.times
    now = Time.now
    Struct::Tms.new(now - $STARTUP_TIME, 0.0, 0.0, 0.0)
  end

  def self.kill(sig, pid)
    use_process_group = false
    if sig.kind_of?(String)
      if sig[0] == 45
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
    pid = -pid if use_process_group
    raise ArgumentError unless number
    ret = Platform::POSIX.kill(pid, number)
    case ret
    when 0
      return 1
    when -1
      Errno.handle
    end
  end

  def self.getpgid(pid)
    ret = Platform::POSIX.getpgid(pid)
    Errno.handle if ret == -1
    ret
  end

  def self.setpgid(pid, int)
    ret = Platform::POSIX.setpgid(pid, int)
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
    ret = Platform::POSIX.getpgrp
    Errno.handle if ret == -1
    ret
  end

  def self.pid
    ret = Platform::POSIX.getpid
    Errno.handle if ret == -1
    ret
  end

  def self.ppid
    ret = Platform::POSIX.getppid
    Errno.handle if ret == -1
    ret
  end

  def self.uid=(uid)
    Process::Sys.setuid uid
  end

  def self.gid=(gid)
    Process::Sys.setgid gid
  end

  def self.euid=(uid)
    Process::Sys.seteuid uid
  end

  def self.egid=(gid)
    Process::Sys.setegid gid
  end

  def self.uid
    ret = Platform::POSIX.getuid
    Errno.handle if ret == -1
    ret
  end

  def self.gid
    ret = Platform::POSIX.getgid
    Errno.handle if ret == -1
    ret
  end

  def self.euid
    ret = Platform::POSIX.geteuid
    Errno.handle if ret == -1
    ret
  end

  def self.egid
    ret = Platform::POSIX.getegid
    Errno.handle if ret == -1
    ret
  end

  def self.getpriority(kind, id)
    Platform::POSIX.errno = 0
    ret = Platform::POSIX.getpriority(kind, id)
    Errno.handle
    ret
  end

  def self.setpriority(kind, id, priority)
    ret = Platform::POSIX.setpriority(kind, id, priority)
    Errno.handle if ret == -1
    ret
  end

  def self.groups
    g = []
    MemoryPointer.new(:int, @maxgroups) { |p|
      num_groups = Platform::POSIX.getgroups(@maxgroups, p)
      Errno.handle if num_groups == -1
      g = p.read_array_of_int(num_groups)
    }
    g
  end

  def self.groups=(g)
    @maxgroups = g.length if g.length > @maxgroups
    MemoryPointer.new(:int, @maxgroups) { |p|
      p.write_array_of_int(g)
      Errno.handle if -1 == Platform::POSIX.setgroups(g.length, p)
    }
    g
  end

  def self.initgroups(username, gid)
    Errno.handle if -1 == Platform::POSIX.initgroups(username, gid)
    Process.groups
  end

  def self.wait(pid=-1, flags=0)
    chan = Channel.new
    Scheduler.send_on_stopped(chan, pid, flags)
    pid, status = chan.receive
    case pid
    when false
      raise Errno::ECHILD
    when nil
      return nil
    else
      $? = Process::Status.new pid, status
    end
    return pid
  end

  def self.waitall
    statuses = []
    statuses << [Process.wait, $?] while true
  rescue Errno::ECHILD
    statuses
  end

  def self.wait2(pid=-1, flags=0)
    pid = Process.wait(pid, flags)
    pid ? [pid, $?] : nil
  end

  class << self
    alias_method :waitpid, :wait
    alias_method :waitpid2, :wait2
  end

  def self.detach(pid)
    thr = Thread.new {
      while true
        break if Process.wait(pid, Process::WNOHANG)
        sleep(1)
      end
    }
  end

  # TODO: Most of the fields aren't implemented yet.
  # TODO: Also, these objects should only need to be constructed by Process.wait and family.
  class Status
    def initialize(pid, status)
      @pid = pid
      @status = status
    end
    
    def to_i
      @status
    end
    
    def to_s
      @status.to_s
    end
    
    def &(num)
      @status & num
    end
    
    def ==(other)
      other = other.to_i if other.kind_of? Process::Status
      @status == other
    end
    
    def >>(num)
      @status >> num
    end
    
    def coredump?
      false
    end
    
    def exited?
      true
    end
    
    def exitstatus
      @status
    end
    
    def pid
      @pid
    end
    
    def signaled?
      false
    end
    
    def stopped?
      false
    end
    
    def stopsig
      nil
    end
    
    def success?
      @status == 0
    end
    
    def termsig
      nil
    end
  end

  module Sys
    class << self
      def getegid
        ret = Platform::POSIX.getegid
        Errno.handle if ret == -1
        ret
      end
      def geteuid
        ret = Platform::POSIX.geteuid
        Errno.handle if ret == -1
        ret
      end
      def getgid
        ret = Platform::POSIX.getgid
        Errno.handle if ret == -1
        ret
      end
      def getuid
        ret = Platform::POSIX.getuid
        Errno.handle if ret == -1
        ret
      end
      def issetugid
        raise "not implemented"
      end
      def setgid(gid)
        Platform::POSIX.setgid gid
        Errno.handle if ret == -1
        nil
      end
      def setuid(uid)
        Platform::POSIX.setuid uid
        Errno.handle if ret == -1
        nil
      end
      def setegid(egid)
        ret = Platform::POSIX.setegid egid
        Errno.handle if ret == -1
        nil
      end
      def seteuid(euid)
        Platform::POSIX.seteuid euid
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
        Platform::POSIX.setregid rid, eid
        Errno.handle if ret == -1
        nil
      end
      def setreuid(rid)
        Platform::POSIX.setreuid rid
        Errno.handle if ret == -1
        nil
      end
      def setresgid(rid, eid, sid)
        Platform::POSIX.setresgid rid, eid, sid
        Errno.handle if ret == -1
        nil
      end
      def setresuid(rid, eig, sid)
        Platform::POSIX.setresuid rid, eid, sid
        Errno.handle if ret == -1
        nil
      end
    end
  end

  module UID
    class << self
      def change_privilege(uid)
        Platform::POSIX.setreuid(uid, uid)
        uid
      end

      def eid
        ret = Platform::POSIX.geteuid
        Errno.handle if ret == -1
        ret
      end

      def eid=(uid)
        ret = Platform::POSIX.seteuid(uid)
        Errno.handle if ret == -1
        uid
      end
      alias_method :grant_privilege, :eid=

      def re_exchange
        real = Platform::POSIX.getuid
        Errno.handle if real == -1
        eff = Platform::POSIX.geteuid
        Errno.handle if eff == -1
        ret = Platform::POSIX.setreuid(eff, real)
        Errno.handle if ret == -1
        eff
      end

      def re_exchangeable?
        true
      end

      def rid
        ret = Platform::POSIX.getuid
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
        ret = Platform::POSIX.setregid(gid, gid)
        Errno.handle if ret == -1
        gid
      end

      def eid
        ret = Platform::POSIX.getegid
        Errno.handle if ret == -1
        ret
      end

      def eid=(gid)
        ret = Platform::POSIX.setegid(gid)
        Errno.handle if ret == -1
        gid
      end
      alias_method :grant_privilege, :eid=

      def re_exchange
        real = Platform::POSIX.getgid
        Errno.handle if real == -1
        eff = Platform::POSIX.getegid
        Errno.handle if eff == -1
        ret = Platform::POSIX.setregid(eff, real)
        Errno.handle if ret == -1
        eff
      end

      def re_exchangeable?
        true
      end

      def rid
        ret = Platform::POSIX.getgid
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
  
  def system(prog, *args)
    cmd = args.inject(prog.to_s) { |a,e| a << " #{e}" }
    pid = Process.fork
    if pid
      Process.waitpid(pid)
      return $?.exitstatus == 0
    else
      Process.replace "/bin/sh", ["sh", "-c", cmd]
    end
  end

  def exec(cmd, *args)
    if args.empty? and cmd.kind_of? String
      raise SystemCallError if cmd.empty?
      Process.replace "/bin/sh", ["sh", "-c", cmd]
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
      
      Process.replace prog, argv
    end
  end
  
  def `(str) #`
    str = StringValue(str)
    read, write = IO.pipe
    pid = Process.fork
    if pid
      write.close
      chan = Channel.new
      output = ""
      buf = String.new(50)
      while true
        Scheduler.send_on_readable chan, read, buf, 50
        res = chan.receive
        if res.nil?
          Process.waitpid(pid)
          return output
        else
          output << buf
        end
      end
    else
      read.close
      STDOUT.reopen write
      Process.replace "/bin/sh", ["sh", "-c", str]
    end
  end
end

class IO
  def self.popen(str, mode="r")
    raise "TODO make this support more than r" if mode != "r"
    
    if str == "+-+" and !block_given?
      raise ArgumentError, "this mode requires a block currently"
    end
    
    pa_read, ch_write = IO.pipe
    
    pid = Process.fork

    if pid
      ch_write.close
      rp = BidirectionalPipe.new(pid, pa_read, nil)
      if block_given?
        begin
          yield rp
        ensure
          pa_read.close
        end
      else
        return rp
      end
    else
      pa_read.close
      STDOUT.reopen ch_write
      if str == "+-+"
        yield nil
      else
        Process.replace "/bin/sh", ["-c", str]
      end
    end
  end
end

class BidirectionalPipe < IO
  def initialize(pid, read, write)
    super(read)
    @pid = pid
    @write = write
  end
  
  def pid
    @pid
  end
  
  def <<(str)
    @write << str
  end
  
  alias_method :write, :<<
  
  def syswrite(str)
    @write.syswrite str
  end
    
end

class Struct
  def self.after_loaded
    Struct.new 'Tms', :utime, :stime, :cutime, :cstime
  end
end


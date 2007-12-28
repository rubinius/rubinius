class Process
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

  def self.fork
    pid = fork_prim
    pid = nil if pid == 0
    if block_given? && pid.nil?
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
    Platform::POSIX.getpgid(pid)
  end

  def self.setpgid(pid, int)
    Platform::POSIX.setpgid(pid, int)
  end

  def self.pid
    Platform::POSIX.getpid
  end

  def self.ppid
    Platform::POSIX.getppid
  end

  def self.uid
    Platform::POSIX.getuid
  end

  def self.gid
    Platform::POSIX.getgid
  end

  def self.wait(pid=-1, flags=0)
    chan = Channel.new
    Scheduler.send_on_stopped(chan, pid, flags)
    pid = chan.receive
    case pid
    when false
      raise Errno::ECHILD
    when nil
      return nil
    else
      status = chan.receive
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
end

module Kernel
  def system(prog, *args)
    cmd = args.inject(prog.to_s) { |a,e| a << " #{e}" }
    pid = Process.fork
    if pid
      Process.waitpid(pid)
      return $?.exitstatus == 0
    else
      Process.replace "/bin/sh", ["-c", cmd]
    end
  end

  def exec(cmd, *args)
    Process.replace "bin/sh", ["-c", cmd, *args]
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
      Process.replace "/bin/sh", ["-c", str]
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


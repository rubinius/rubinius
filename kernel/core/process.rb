class Process    
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
    if pid != 0
      chan = Channel.new
      Scheduler.send_on_stopped chan, pid
      status = chan.receive
      $? = Process::Status.new pid, status
      return false if status != 0
      
      return true
    else
      Process.replace "/bin/sh", ["-c", cmd]
    end
  end
  
  def `(str)
    read, write = IO.pipe
    pid = Process.fork
    if pid != 0
      write.close
      chan = Channel.new
      output = ""
      buf = String.new(50)
      while true
        Scheduler.send_on_readable chan, read, buf, 50
        res = chan.receive
        if res.nil?
          Scheduler.send_on_stopped chan, pid
          $? = Process::Status.new pid, chan.receive
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
  def popen(str, mode="r")
    raise "TODO make this support more than r" if mode != "r"
    
    if str == "+-+" and !block_given?
      raise ArgumentError, "this mode requires a block currently"
    end
    
    pa_read, ch_write = IO.pipe
    
    pid = Process.fork
    
    if pid != 0
      ch_write.close
      rp = BidirectonalPipe.new(pid, pa_read, nil)
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

class BidirectionalPipe
  def initialize(pid, read, write)
    @pid = pid
    @read = read
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
  
  def read(count)
    @read.read(count)
  end
  
  def sysread(count)
    @read.sysread(count)
  end
  
  def method_missing(m, *a, &b)
    @read.__send__(m, *a, &b)
  end
end


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
end

module Kernel
  def system(prog, *args)
    cmd = args.inject(prog.to_s) { |a,e| a << " #{e}" }
    pid = Process.fork
    if pid != 0
      chan = Channel.new
      Scheduler.send_on_stopped chan, pid
      status = chan.receive
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
        if String === res
          output << res
        elsif !res
          Scheduler.send_on_stopped chan, pid
          res = chan.receive
          return output
        end
      end
    else
      read.close
      STDOUT.reopen write
      Process.replace "/bin/sh", ["-c", str]
    end
  end
end

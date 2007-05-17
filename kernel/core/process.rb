class Process
  def self.exit(code)
    Ruby.primitive :process_exit
  end
  
  def self.micro_sleep(ms)
    Ruby.primitive :micro_sleep
  end
  
  def self.sleep(sec)
    micro_sleep(sec * 1_000_000)
  end
  
  def self.usleep(sec)
    micro_sleep(sec * 1_000)
  end

  def self.fork
    Ruby.primitive :fork_process
  end

  def self.replace(file, arg)
    Ruby.primitive :replace_process
  end
end

module Kernel
  def system(prog, *args)
    cmd = args.inject(prog.to_s) { |a,e| a << " #{e}" }
    pid = Process.fork
    if pid != 0
      chan = Channel.new
      chan.send_on_stopped pid
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
        chan.send_on_readable read, buf, 50
        res = chan.receive
        if String === res
          output << res
        elsif !res
          chan.send_on_stopped pid
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

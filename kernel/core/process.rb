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
end

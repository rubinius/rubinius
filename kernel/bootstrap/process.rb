class Process
  def self.exit(code)
    Ruby.primitive :process_exit
    raise PrimitiveFailure, "exit failed. Wow, something is screwed."
  end
  
  # MTP
  def self.micro_sleep(ms)
    Ruby.primitive :micro_sleep
  end
  
  # MTP
  def self.fork
    Ruby.primitive :fork_process
  end

  # MTP
  def self.replace(file, arg)
    Ruby.primitive :replace_process
  end
  
end

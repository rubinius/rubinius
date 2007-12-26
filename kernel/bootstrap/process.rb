class Process
  def self.exit(code)
    Ruby.primitive :process_exit
    raise PrimitiveFailure, "exit failed. Wow, something is screwed."
  end
  
  def self.micro_sleep(ms)
    Ruby.primitive :micro_sleep
  end

  def self.fork_prim
    Ruby.primitive(:fork_process)
  end

  def self.replace(file, arg)
    Ruby.primitive :replace_process
    raise Errno::ENOENT, "No such file or directory - #{file}"
  end
end

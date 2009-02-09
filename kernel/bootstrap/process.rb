class Process
  def self.perform_fork
    Ruby.primitive :vm_fork
  end

  def self.perform_exec(file, args)
    Ruby.primitive :vm_exec
  end

  def self.wait_pid_prim(pid, no_hang)
    Ruby.primitive :vm_wait_pid
  end
end

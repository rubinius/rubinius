module Process
  def self.perform_fork
    Ruby.primitive :vm_fork
    raise PrimitiveFailure, "vm_fork primitive failed"
  end

  def self.perform_exec(file, args)
    Ruby.primitive :vm_exec
    raise PrimitiveFailure, "vm_exec primitive failed"
  end

  def self.replace(str)
    Ruby.primitive :vm_replace
    raise PrimitiveFailure, "vm_replace primitive failed"
  end

  def self.wait_pid_prim(pid, no_hang)
    Ruby.primitive :vm_wait_pid
    raise PrimitiveFailure, "vm_wait_pid primitive failed"
  end

  def self.time
    Ruby.primitive :vm_time
    raise PrimitiveFailure, "vm_time primitive failed"
  end

  def self.cpu_times
    Ruby.primitive :vm_times
    raise PrimitiveFailure, "vm_times primitive failed"
  end
end

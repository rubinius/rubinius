module Process
  def self.wait_pid_prim(pid, no_hang)
    Rubinius.primitive :vm_wait_pid
    raise PrimitiveFailure, "Process.wait_pid primitive failed"
  end

  def self.time
    Rubinius.primitive :vm_time
    raise PrimitiveFailure, "Process.time primitive failed"
  end

  def self.cpu_times
    Rubinius.primitive :vm_times
    raise PrimitiveFailure, "Process.cpu_times primitive failed"
  end
end

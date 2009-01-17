class Process
  def self.perform_fork
    Ruby.primitive :vm_fork
  end

  def self.perform_exec(file, args)
    Ruby.primitive :vm_exec
  end
end

class Process
  def self.exit(code)
    Ruby.primitive :vm_exit
    raise PrimitiveFailure, "exit() failed. Wow, something is screwed."
  end

  def self.perform_fork
    Ruby.primitive :vm_fork
  end

  def self.perform_exec(file, args)
    Ruby.primitive :vm_exec
  end
end

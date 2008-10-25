class Thread

  def self.current
    Ruby.primitive :thread_current
    Kernel.raise PrimitiveFailure, "Failed to get current thread. UH-OH!"
  end

  def self.allocate
    Ruby.primitive :thread_allocate
    Kernel.raise PrimitiveFailure, "Failed to allocate thread"
  end

  def exited
    Ruby.primitive :thread_exited
    Kernel.raise PrimitiveFailure, "Failed to set thread exited"
  end

  def pass
    Ruby.primitive :thread_pass
    Kernel.raise PrimitiveFailure, "Failed to pass"
  end

  def raise_prim(exc)
    Ruby.primitive :thread_raise
    Kernel.raise PrimitiveFailure, "Failed to raise in thread"
  end

  def wakeup
    Ruby.primitive :thread_wakeup
    Kernel.raise ThreadError, "Wakeup failed, thread may be dead"
  end

end

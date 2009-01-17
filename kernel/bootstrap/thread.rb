class Thread

  def self.current
    Ruby.primitive :thread_current
    Kernel.raise PrimitiveFailure, "Threadcurrent primitive failed"
  end

  def self.allocate
    Ruby.primitive :thread_allocate
    Kernel.raise PrimitiveFailure, "Thread.allocate primitive failed"
  end

  def exited
    Ruby.primitive :thread_exited
    Kernel.raise PrimitiveFailure, "Thread#exited primitive failed"
  end

  def pass
    Ruby.primitive :thread_pass
    Kernel.raise PrimitiveFailure, "Thread#pass primitive failed"
  end

  def raise_prim(exc)
    Ruby.primitive :thread_raise
    Kernel.raise PrimitiveFailure, "Thread#raise primitive failed"
  end

  def wakeup
    Ruby.primitive :thread_wakeup
    Kernel.raise ThreadError, "Thread#wakeup primitive failed, thread may be dead"
  end

end

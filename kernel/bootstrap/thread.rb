class Thread

  def self.current
    Ruby.primitive :thread_current
    Kernel.raise PrimitiveFailure, "Threadcurrent primitive failed"
  end

  def self.allocate
    Ruby.primitive :thread_allocate
    Kernel.raise PrimitiveFailure, "Thread.new primitive failed"
  end

  def self.pass
    Ruby.primitive :thread_pass
    Kernel.raise PrimitiveFailure, "Thread#pass primitive failed"
  end

  def fork
    Ruby.primitive :thread_fork
    Kernel.raise PrimitiveFailure, "Thread#fork primitive failed"
  end

  def raise_prim(exc)
    Ruby.primitive :thread_raise
    Kernel.raise PrimitiveFailure, "Thread#raise primitive failed"
  end

  def wakeup
    Ruby.primitive :thread_wakeup
    Kernel.raise ThreadError, "Thread#wakeup primitive failed, thread may be dead"
  end

  def priority
    Ruby.primitive :thread_priority
    Kernel.raise ThreadError, "Unable to get Thread priority"
  end

  def priority=(val)
    Ruby.primitive :thread_set_priority
    Kernel.raise ThreadError, "Unable to set Thread priority"
  end

  def __context__
    Ruby.primitive :thread_context
    Kernel.raise PrimitiveFailure, "Thread#__context__ failed"
  end

  def native_join
    Ruby.primitive :thread_join
    Kernel.raise PrimitiveFailure, "Thread#native_join failed"
  end

  def self.set_critical(obj)
    Ruby.primitive :thread_set_critical
    Kernel.raise PrimitiveFailure, "Thread.set_critical failed"
  end

end

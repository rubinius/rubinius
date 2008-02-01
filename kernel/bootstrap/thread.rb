class Thread
  def self.current
    Ruby.primitive :thread_current
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def self.allocate
    Ruby.primitive :thread_new
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def self.pass
    Ruby.primitive :thread_yield
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def self.dequeue
    Ruby.primitive :thread_dequeue
    Kernel.raise PrimitiveFailure, "primitive failed"
  end
  
  def run
    Ruby.primitive :thread_run
    Kernel.raise ThreadError, "killed thread"
  end

  def wakeup
    Ruby.primitive :thread_schedule
    Kernel.raise PrimitiveFailure, "primitive failed"
  end
  
end

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


  #
  # Sleep current thread for defined number of seconds or indefinitely.
  #
  # The sleep can be interrupted by an explicit #run call on the Thread
  # in question.
  #
  # It is only possible to sleep the current thread, because doing so
  # for any other Thread is dangerous and complicated.
  #
  def self.sleep(arg)
    Ruby.primitive :thread_sleep
    sleep(arg.to_i)
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

end

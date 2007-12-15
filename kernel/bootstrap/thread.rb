class Thread
  def self.current
    Ruby.primitive :thread_current
  end

  def self.allocate
    Ruby.primitive :thread_new
  end

  def self.pass
    Ruby.primitive :thread_yield
  end
  
  def run
    Ruby.primitive :thread_run
    Kernel.raise ThreadError, "killed thread"
  end

  def wakeup
    Ruby.primitive :thread_schedule
  end
  
end
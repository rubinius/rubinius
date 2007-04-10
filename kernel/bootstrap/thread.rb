class Thread
  
  ivar_as_index :__ivars__ => 0, :priority => 1, :task => 2, :joins => 3

  def initialize(&prc)
    raise "Must pass in a block" unless prc
    @task.associate prc.block
  end
  
  def self.new(&block)
    th = allocate()
    th.initialize(&block)
    th.wakeup
    return th
  end

  def self.current
    Ruby.primitive :thread_current
  end

  def self.allocate
    Ruby.primitive :thread_new
  end

  def self.yield
    Ruby.primitive :thread_yield
  end

  def run
    Ruby.primitive :thread_run
  end

  def wakeup
    Ruby.primitive :thread_schedule
  end
end

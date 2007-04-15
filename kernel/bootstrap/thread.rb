class Thread
  
  ivar_as_index :__ivars__ => 0, :priority => 1, :task => 2, :joins => 3

  def initialize(&prc)
    raise "Must pass in a block" unless prc
    setup_task do
      prc.call
      @joins.each do |ch|
        ch.send self
      end
    end
  end

  def setup_task(&prc)
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

  def join(time=nil)
    if time
      tc = Channel.new
      tc.send_in_microseconds(time * 1_000_000)
    end
    
    jc = Channel.new
    @joins << jc

    if time
      tup = Channel.receive_many([tc, jc])
      return nil if tup[0] == tc
    else
      jc.receive
    end

    return self
  end

  def self.sleep(secs)
    chan = Channel.new
    chan.send_in_microseconds(secs * 1_000_000)
    chan.receive
    return true
  end
end

class Thread
  ivar_as_index :__ivars__ => 0, :priority => 1, :task => 2, :joins => 3

  def setup(prime_lock)
    @__ivars__ = {}
    @alive = true
    @result = nil
    @exception = nil
    @lock = Channel.new
    @lock.send nil if prime_lock
  end

  def initialize(&prc)
    raise "Must pass in a block" unless prc
    setup(false)
    setup_task do
      begin
        begin
          @lock.send nil
          @result = prc.call
        ensure
          @lock.receive
          @alive = false
          @joins.each do |join|
            join.send self
          end
        end
      rescue Exception => e
        @exception = e
      ensure
        @lock.send nil
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

  def alive?
    @lock.receive
    begin
      @alive
    ensure
      @lock.send nil
    end
  end

  def status
    if alive?
      "run"
    else
      false
    end
  end

  def run
    Ruby.primitive :thread_run
  end

  def wakeup
    Ruby.primitive :thread_schedule
  end

  def join(timeout=nil)
    join_inner(timeout) do
      break nil if @alive
      self
    end
  end

  def value
    join_inner(nil) { @result }
  end

  def join_inner(timeout)
    result = nil
    @lock.receive
    begin
      if @alive
        jc = Channel.new
        @joins << jc
        @lock.send nil
        begin
          Scheduler.send_in_microseconds(jc, timeout * 1_000_000) if timeout
          jc.receive
        ensure
          @lock.receive
        end
      end
      raise @exception if @exception
      result = yield
    ensure
      @lock.send nil
    end
    result
  end

  def self.sleep(secs)
    chan = Channel.new
    Scheduler.send_in_microseconds(chan, secs * 1_000_000)
    chan.receive
    return true
  end

  def [](key)
    @__ivars__[key]
  end

  def []=(key, value)
    @__ivars__[key] = value
  end
end

Thread.current.setup(true)


class Thread
  ivar_as_index :__ivars__ => 0, :priority => 1, :task => 2, :joins => 3

  def setup
    @__ivars__ = {}
    @alive = true
    @result = nil
    @exception = nil
    @lock = Channel.new
  end

  def initialize(&prc)
    raise "Must pass in a block" unless prc
    setup
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

  def join(time=nil)
    value(time)
    self
  end

  def value(time=nil)
    @lock.receive
    begin
      if @alive
        jc = Channel.new
        @joins << jc
        @lock.send nil
        begin
          if time
            tc = Channel.new
            tc.send_in_microseconds(time * 1_000_000)
            tup = Channel.receive_many([tc, jc])
            return nil if tup[0] == tc
          else
            jc.receive
          end
        ensure
          @lock.receive
        end
      end
      raise @exception if @exception
      @result
    ensure
      @lock.send nil
    end
  end

  def self.sleep(secs)
    chan = Channel.new
    chan.send_in_microseconds(secs * 1_000_000)
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

Thread.current.setup


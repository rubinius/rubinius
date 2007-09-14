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

  def initialize(*args)
    raise ThreadError.new("must be called with a block") unless block_given?
    raise ThreadError.new("thread block arguments not yet supported") unless args.empty?
    setup(false)
    setup_task do
      begin
        begin
          @lock.send nil
          @result = yield
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

  def setup_task
    block = Ruby.asm "push_block"
    @task.associate block
  end
  
  def self.new
    block = Ruby.asm "push_block"
    th = allocate()
    th.initialize(&block)
    th.wakeup
    return th
  end
  
  def current_context
    @task.current_context
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
  
  def raise(exc=$!, msg=nil, trace=nil)
    if exc.respond_to? :exception
      exc = exc.exception msg
      raise TypeError, 'exception class/object expected' unless Exception === exc
      exc.set_backtrace trace if trace
    elsif exc.kind_of? String or !exc
      exc = RuntimeError.exception exc
    else
      raise TypeError, 'exception class/object expected'
    end
    
    if $DEBUG
      STDERR.puts "Exception: #{exc.message} (#{exc.class})"
    end
    
    ctx = @task.current_context

    exc.set_backtrace ctx unless exc.backtrace
    
    @task.raise exc
  end

  def [](key)
    @__ivars__[key]
  end

  def []=(key, value)
    @__ivars__[key] = value
  end
  
  def set_debugging(dc, cc)
    @task.set_debugging(dc, cc)
  end

  def self.main
    @main_thread
  end

  def self.initialize_main_thread(thread)
    @main_thread = thread
  end
end

Thread.current.setup(true)
Thread.initialize_main_thread(Thread.current)



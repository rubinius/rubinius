# Be very careful about calling raise in here! Thread has it's own
# raise which, if you're calling raise, you probably don't want. Use
# Kernel.raise to call the proper raise.

class Thread
  ivar_as_index :__ivars__ => 0, :priority => 1, :task => 2, :joins => 3

  def inspect
    "#<#{self.class}:0x#{object_id.to_s(16)}>"
  end

  def setup(prime_lock)
    @__ivars__ = {}
    @group = nil
    @alive = true
    @result = nil
    @exception = nil
    @lock = Channel.new
    @lock.send nil if prime_lock
  end

  def initialize(*args)
    unless block_given?
      Kernel.raise ThreadError, "must be called with a block"
    end

    block = Ruby.asm "push_block"
    block.disable_long_return!

    setup(false)
    setup_task do
      begin
        begin
          @lock.send nil
          begin
            @result = block.call *args
          rescue IllegalLongReturn => e2
            Kernel.raise ThreadError, 
                      "return is not allowed across threads", e2.context
          end
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
  
  def self.new(*args)
    block = Ruby.asm "push_block"
    th = allocate()
    th.initialize(*args, &block)
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
  
  def group
    @group
  end
  
  def add_to_group(group)
    @group = group
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
      Kernel.raise @exception if @exception
      result = yield
    ensure
      @lock.send nil
    end
    result
  end

  def self.sleep(duration = nil)
    Thread.current.sleep(duration)
  end
  
  def sleep(duration = nil)
    chan = Channel.new
    start = Time.now
    Scheduler.send_in_microseconds(chan, (duration * 1_000_000).to_i) if(duration)
    chan.receive
    return (Time.now - start)
  end
  
  def raise(exc=$!, msg=nil, trace=nil)
    if exc.respond_to? :exception
      exc = exc.exception msg
      Kernel.raise TypeError, 'exception class/object expected' unless Exception === exc
      exc.set_backtrace trace if trace
    elsif exc.kind_of? String or !exc
      exc = RuntimeError.exception exc
    else
      Kernel.raise TypeError, 'exception class/object expected'
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



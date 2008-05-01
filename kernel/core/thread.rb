# depends on: class.rb

#--
# Be very careful about calling raise in here! Thread has it's own
# raise which, if you're calling raise, you probably don't want. Use
# Kernel.raise to call the proper raise.
#++

class Thread

  class Die < Exception; end # HACK

  ivar_as_index :__ivars__ => 0, :priority => 1, :task => 2, :joins => 3
  def task; @task; end

  @abort_on_exception = false

  def self.abort_on_exception
    @abort_on_exception
  end

  def self.abort_on_exception=(val)
    @abort_on_exception = val
  end

  def inspect
    stat = status()
    stat = "dead" unless stat

    "#<#{self.class}:0x#{object_id.to_s(16)} #{stat}>"
  end

  def setup(prime_lock)
    @group = nil
    @sleep = true
    @alive = true
    @result = nil
    @exception = nil
    @critical = false
    @locals = LookupTable.new
    @lock = Channel.new
    @lock.send nil if prime_lock
  end

  def initialize(*args)
    unless block_given?
      Kernel.raise ThreadError, "must be called with a block"
    end

    block = block_given?
    block = block.block if block.kind_of? Proc
    block.disable_long_return!

    setup(false)
    setup_task do
      begin
        begin
          @lock.send nil
          begin
            @result = block.call(*args)
          rescue IllegalLongReturn, LongReturnException => e2
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
      rescue Die
        @exception = nil
      rescue Exception => e
        @exception = e
      ensure
        @lock.send nil
      end

      if @exception
        if Thread.abort_on_exception
          Thread.main.raise @exception
        elsif $DEBUG
          STDERR.puts "Exception in thread: #{@exception.message} (#{@exception.class})"
        end
      end

      Thread.dequeue
    end

    Thread.current.group.add self

  end

  def setup_task
    block = block_given?
    @task.associate block
  end

  def self.new(*args)
    block = block_given?
    th = allocate()
    th.__send__ :initialize, *args, &block
    th.wakeup
    return th
  end

  def self.start(*args, &block)
    new(*args, &block) # HACK
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

  def stop?
    !alive? || @sleep
  end

  def kill
    raise Die
  end

  def status
    if alive?
      if @sleep
        "sleep"
      else
        "run"
      end
    else
      if(@exception)
        nil
      else
        false
      end
    end
  end

  def self.stop()
    Thread.critical = false
    sleep
    nil
  end

  def self.critical
    @critical
  end

  def self.critical=(value)
    @critical = value
  end

  def join(timeout = Undefined)
    join_inner(timeout) { @alive ? nil : self }
  end

  def group
    @group
  end

  def add_to_group(group)
    @group = group
  end

  def value
    join_inner { @result }
  end

  def join_inner(timeout = Undefined)
    result = nil
    @lock.receive
    begin
      if @alive
        jc = Channel.new
        @joins << jc
        @lock.send nil
        begin
          unless timeout.equal?(Undefined)
            Scheduler.send_in_seconds(jc, timeout.to_f, nil)
          end
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
  private :join_inner

  def raise_prim(exc)
    Ruby.primitive :thread_raise
  end
  private :raise_prim

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

    raise_prim exc
  end

  def [](key)
    raise TypeError,     "#{key} is not a symbol" if key.kind_of?(NilClass)
    raise ArgumentError, "#{key} is not a symbol" unless key.kind_of?(Symbol) or key.kind_of?(String)
    @locals[Type.coerce_to(key,Symbol,:to_sym)]
  end

  def []=(key, value)
    raise TypeError,     "#{key} is not a symbol" if key.kind_of?(NilClass)
    raise ArgumentError, "#{key} is not a symbol" unless key.kind_of?(Symbol) or key.kind_of?(String)
    @locals[Type.coerce_to(key,Symbol,:to_sym)] = value
  end

  def keys
    @locals.keys
  end

  def key?(key)
    raise TypeError,     "#{key} is not a symbol" if key.kind_of?(NilClass)
    raise ArgumentError, "#{key} is not a symbol" unless key.kind_of?(Symbol) or key.kind_of?(String)
    @locals.key?(Type.coerce_to(key,Symbol,:to_sym))
  end

  def set_debugging(dc, cc)
    @task.set_debugging(dc, cc)
  end

  def debug_channel
    @task.debug_channel
  end

  def control_channel
    @task.control_channel
  end

  def self.main
    @main_thread
  end

  def self.initialize_main_thread(thread)
    @main_thread = thread
  end

  def self.after_loaded
    Thread.current.setup(true)
    Thread.initialize_main_thread(Thread.current)
    dg = ThreadGroup.new
    ThreadGroup.const_set :Default, dg
    dg.add Thread.current
  end
end

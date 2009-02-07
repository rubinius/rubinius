# depends on: class.rb

#--
# Be very careful about calling raise in here! Thread has its own
# raise which, if you're calling raise, you probably don't want. Use
# Kernel.raise to call the proper raise.
#++

class Thread

  class Die < Exception; end # HACK

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

  def self.start(*args, &block)
    new(*args, &block) # HACK
  end

  def initialize(*args, &block)
    setup(false)
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
        @joins.each {|join| join.send self }
      end
    rescue Die
      @exception = nil
    rescue Exception => e
      @exception = e
    ensure
      @lock.send nil
    end

    begin
      if @exception
        puts "#{@exception.message} (#{@exception.class})"
        puts @exception.awesome_backtrace.show

        if Thread.abort_on_exception
          Thread.main.raise @exception
        elsif $DEBUG
          STDERR.puts "Exception in thread: #{@exception.message} (#{@exception.class})"
        end
      end
    end
  end

  def setup(prime_lock)
    @group = nil
    @alive = true
    @result = nil
    @exception = nil
    @critical = false
    @locals = LookupTable.new
    @lock = Channel.new
    @lock.send nil if prime_lock
    @joins = []
  end

  def setup_task
    block = block_given?
    @task.associate block
  end

  def current_context
    @task.current_context
  end

  def alive?
    @lock.receive
    @alive
  ensure
    @lock.send nil
  end

  def stop?
    !alive? || @sleep
  end

  def kill
    self.raise Die
  end

  alias_method :exit, :kill
  alias_method :terminate, :kill

  def sleeping?
    @lock.receive
    @sleep
  ensure
    @lock.send nil
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

    Kernel.raise exc if self == Thread.current

    raise_prim exc
  end

  def [](key)
    @locals[Type.coerce_to_symbol(key)]
  end

  def []=(key, value)
    @locals[Type.coerce_to_symbol(key)] = value
  end

  def keys
    @locals.keys
  end

  def key?(key)
    @locals.key?(Type.coerce_to_symbol(key))
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

  def self.list
    Thread.current.group.list
  end

  private :raise_prim

  alias_method :run, :wakeup
end

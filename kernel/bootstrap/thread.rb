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

  def __context__
    Ruby.primitive :thread_context
    Kernel.raise PrimitiveFailure, "Thread#__context__ failed"
  end


  # Don't move the code below here to common, it is entirely
  # Rubinius specific in how we make Threads work.
  #
  # kernel/common/thread.rb is only for non-Rubinius specific code.

  class Die < Exception; end # HACK

  @abort_on_exception = false

  def self.abort_on_exception
    @abort_on_exception
  end

  def self.abort_on_exception=(val)
    @abort_on_exception = val
  end

  # It's also an instance method...
  def abort_on_exception=(val)
    @abort_on_exception = val
  end

  def abort_on_exception
    @abort_on_exception ||= false
  end

  def inspect
    stat = status()
    stat = "dead" unless stat

    "#<#{self.class}:0x#{object_id.to_s(16)} #{stat}>"
  end

  def self.new(*args, &block)
    thr = allocate
    thr.send(:initialize, *args, &block)
    thr.fork

    return thr
  end

  class << self
    alias_method :start, :new
  end

  def initialize(*args, &block)
    Kernel.raise ThreadError, "must be called with a block" unless block_given?
    setup(false)
    @args = args
    @block = block

    Thread.current.group.add self
  end

  # Called by Thread#fork in the new thread
  #
  def __run__()
    begin
      begin
        @lock.send nil
        @result = @block.call(*@args)
      ensure
        @lock.receive
        @joins.each { |join| join.send self }
      end
    rescue Die
      @exception = nil
    rescue Exception => e
      # I don't really get this, but this is MRI's behavior. If we're dying
      # by request, ignore any raised exception.
      @exception = e # unless @dying
    ensure
      @alive = false
      @lock.send nil
    end

    if @exception
      if abort_on_exception or Thread.abort_on_exception
        Thread.main.raise @exception
      elsif $DEBUG
        STDERR.puts "Exception in thread: #{@exception.message} (#{@exception.class})"
      end
    end
  end

  def setup(prime_lock)
    @group = nil
    @alive = true
    @result = false
    @exception = nil
    @critical = false
    @dying = false
    @locals = Rubinius::LookupTable.new
    @lock = Rubinius::Channel.new
    @lock.send nil if prime_lock
    @joins = []
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
    @dying = true
    @sleep = false
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
    if @alive
      if @sleep
        "sleep"
      elsif @dying
        "aborting"
      else
        "run"
      end
    elsif @exception
      nil
    else
      false
    end
  end

  def self.stop
    # Make sure that if we're stopping the current Thread,
    # others can run, so reset critical.
    Thread.critical = false
    sleep
    nil
  end

  def self.critical
    @critical
  end

  def self.critical=(value)
    @critical = !!value
  end

  def join(timeout = undefined)
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

  def join_inner(timeout = undefined)
    result = nil
    @lock.receive
    begin
      if @alive
        jc = Rubinius::Channel.new
        @joins << jc
        @lock.send nil
        begin
          if timeout.equal? undefined
            while true
              res = jc.receive
              # receive returns false if it was a spurious wakeup
              break if res != false
            end
          else
            duration = timeout.to_f
            while true
              start = Time.now.to_f
              res = jc.receive_timeout duration
              # receive returns false if it was a spurious wakeup
              break if res != false
              elapsed = Time.now.to_f - start
              duration -= elapsed
              break if duration < 0
            end
          end
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
    @lock.receive

    unless @alive
      @lock.send nil
      return self
    end

    begin
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
    ensure
      @lock.send nil
    end

    raise_prim exc
  end
  private :raise_prim

  def [](key)
    @locals[Rubinius::Type.coerce_to_symbol(key)]
  end

  def []=(key, value)
    @locals[Rubinius::Type.coerce_to_symbol(key)] = value
  end

  def keys
    @locals.keys
  end

  def key?(key)
    @locals.key?(Rubinius::Type.coerce_to_symbol(key))
  end

  # Register another Thread object +thr+ as the Thread where the debugger
  # is running. When the current thread hits a breakpoint, it uses this
  # field to figure out who to send variable/scope information to.
  #
  def set_debugger_thread(thr)
    raise TypeError, "Must be another Thread" unless thr.kind_of?(Thread)

    @debugger_thread = thr
  end

  # Called by a debugger thread (a thread where the debugger lives) to
  # setup the @control_channel to a Channel object. A debuggee thread
  # will send data to this Channel when it hits a breakpoint, allowing
  # an easy place for the debugger to wait.
  #
  # This channel is sent a Tuple containing:
  #  * The object registered in the breakpoint (can be any object)
  #  * The Thread object where the breakpoint was hit
  #  * A Channel object to use to wake up the debuggee thread
  #  * An Array of Rubinius::Location objects for the stack. These Location
  #    objects contain references to Rubinius::VariableScope objects
  #    which contain info like local variables.
  #
  def setup_control!(chan=nil)
    chan ||= Rubinius::Channel.new
    @control_channel = chan
    return chan
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

  alias_method :run, :wakeup

  class Context
    attr_reader :ip
    attr_reader :method
    attr_reader :variables

    def initialize(ip, method, variables)
      @ip = ip
      @method = method
      @variables = variables
    end

    def file
      @method.file
    end

    def line
      @method.line_from_ip @ip
    end

    def locals
      @variables.locals
    end
  end

  def context
    Context.new(*__context__)
  end

  alias_method :current_context, :context
end

# -*- encoding: us-ascii -*-

class Thread

  def self.current
    Rubinius.primitive :thread_current
    Kernel.raise PrimitiveFailure, "Thread.current primitive failed"
  end

  def self.allocate
    raise TypeError, "allocator undefined for Thread"
  end

  def self.pass
    Rubinius.primitive :thread_pass
    Kernel.raise PrimitiveFailure, "Thread.pass primitive failed"
  end

  def self.list
    Rubinius.primitive :thread_list
    Kernel.raise PrimitiveFailure, "Thread.list primitive failed"
  end

  def fork
    Rubinius.primitive :thread_fork
    Kernel.raise ThreadError, "Thread#fork failed, thread already started or dead"
  end

  def raise_prim(exc)
    Rubinius.primitive :thread_raise
    Kernel.raise PrimitiveFailure, "Thread#raise primitive failed"
  end

  def kill_prim
    Rubinius.primitive :thread_kill
    Kernel.raise PrimitiveFailure, "Thread#kill primitive failed"
  end

  def wakeup
    Rubinius.primitive :thread_wakeup
    Kernel.raise ThreadError, "Thread#wakeup primitive failed, thread may be dead"
  end

  def priority
    Rubinius.primitive :thread_priority
    Kernel.raise ThreadError, "Thread#priority primitive failed"
  end

  def priority=(val)
    Rubinius.primitive :thread_set_priority
    Kernel.raise ThreadError, "Thread#priority= primitive failed"
  end

  def __context__
    Rubinius.primitive :thread_context
    Kernel.raise PrimitiveFailure, "Thread#__context__ primitive failed"
  end

  def native_join
    Rubinius.primitive :thread_join
    Kernel.raise PrimitiveFailure, "Thread#native_join primitive failed"
  end

  def mri_backtrace
    Rubinius.primitive :thread_mri_backtrace
    Kernel.raise PrimitiveFailure, "Thread#mri_backtrace primitive failed"
  end

  def unlock_locks
    Rubinius.primitive :thread_unlock_locks
    Kernel.raise PrimitiveFailure, "Thread#unlock_locks primitive failed"
  end

  def current_exception
    Rubinius.primitive :thread_current_exception
    Kernel.raise PrimitiveFailure, "Thread#current_exception primitive failed"
  end

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
    @abort_on_exception || false
  end

  def inspect
    stat = status()
    stat = "dead" unless stat

    "#<#{self.class}:0x#{object_id.to_s(16)} id=#{@thread_id} #{stat}>"
  end

  alias_method :to_s, :inspect

  def self.new(*args)
    thr = Rubinius.invoke_primitive :thread_allocate, self

    Rubinius::VariableScope.of_sender.locked!

    Rubinius.asm(args, thr) do |args, obj|
      run obj
      dup

      run args
      push_block
      send_with_splat :initialize, 0, true
      # no pop here, as .asm blocks imply a pop as they're not
      # allowed to leak a stack value
    end

    unless thr.thread_is_setup?
      raise ThreadError, "Thread#initialize not called"
    end

    return thr
  end

  def initialize(*args, &block)
    unless block
      Kernel.raise ThreadError, "no block passed to Thread#initialize"
    end

    @args = args
    @block = block

    th_group = Thread.current.group

    th_group.add self

    fork
  end

  alias_method :__thread_initialize__, :initialize

  def thread_is_setup?
    @block != nil
  end

  def alive?
    Rubinius.synchronize(self) do
      @alive
    end
  end

  def stop?
    !alive? || @sleep
  end

  def sleeping?
    Rubinius.synchronize(self) do
      @sleep
    end
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

  def join(timeout = undefined)
    join_inner(timeout) { @alive ? nil : self }
  end

  def group
    @group
  end

  def add_to_group(group)
    @group = group
  end

  def join_inner(timeout = undefined)
    result = nil
    Rubinius.lock(self)
    begin
      if @alive
        jc = Rubinius::Channel.new
        @joins << jc
        Rubinius.unlock(self)
        begin
          if undefined.equal? timeout
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
          Rubinius.lock(self)
        end
      end
      Kernel.raise @exception if @exception
      result = yield
    ensure
      Rubinius.unlock(self)
    end
    result
  end
  private :join_inner

  def raise(exc=undefined, msg=nil, trace=nil)
    Rubinius.lock(self)

    unless @alive
      Rubinius.unlock(self)
      return self
    end

    begin
      if undefined.equal? exc
        no_argument = true
        exc = active_exception
      end

      if exc.respond_to? :exception
        exc = exc.exception msg
        Kernel.raise TypeError, 'exception class/object expected' unless Exception === exc
        exc.set_backtrace trace if trace
      elsif no_argument
        exc = RuntimeError.exception nil
      elsif exc.kind_of? String
        exc = RuntimeError.exception exc
      else
        Kernel.raise TypeError, 'exception class/object expected'
      end

      if $DEBUG
        STDERR.puts "Exception: #{exc.message} (#{exc.class})"
      end

      if self == Thread.current
        Kernel.raise exc
      else
        raise_prim exc
      end
    ensure
      Rubinius.unlock(self)
    end
  end
  private :raise_prim

  def [](key)
    locals_aref(Rubinius::Type.coerce_to_symbol(key))
  end

  def locals_aref(key)
    Rubinius.primitive :thread_locals_aref
    raise PrimitiveFailure, "Thread#locals_aref primitive failed"
  end
  private :locals_aref

  def []=(key, value)
    locals_store(Rubinius::Type.coerce_to_symbol(key), value)
  end

  def locals_store(key, value)
    Rubinius.primitive :thread_locals_store
    raise PrimitiveFailure, "Thread#locals_store primitive failed"
  end
  private :locals_store

  def keys
    Rubinius.primitive :thread_locals_keys
    raise PrimitiveFailure, "Thread#keys primitive failed"
  end

  def key?(key)
    locals_key?(Rubinius::Type.coerce_to_symbol(key))
  end

  def locals_key?(key)
    Rubinius.primitive :thread_locals_has_key
    raise PrimitiveFailure, "Thread#locals_key? primitive failed"
  end
  private :locals_key?

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

  def self.exit
    Thread.current.kill
  end

  def self.kill(thread)
    thread.kill
  end

  alias_method :run, :wakeup

end

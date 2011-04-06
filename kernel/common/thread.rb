#--
# Be very careful about calling raise in here! Thread has its own
# raise which, if you're calling raise, you probably don't want. Use
# Kernel.raise to call the proper raise.
#++

class Thread

  attr_reader :recursive_objects

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

    "#<#{self.class}:0x#{object_id.to_s(16)} id=#{@thread_id} #{stat}>"
  end

  alias_method :to_s, :inspect

  def self.new(*args, &block)
    thr = allocate

    thr.send(:initialize, *args, &block)
    begin
      thr.fork
    rescue Exception => e
      thr.group.remove self
      raise e
    end

    return thr
  end

  def self.start(*args, &block)
    new(*args, &block) # HACK
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
        unlock_locks
        @joins.each {|join| join.send self }
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
    else
      if @exception
        nil
      else
        false
      end
    end
  end

  def self.stop
    # I don't understand at all what this does.
    Thread.critical = false
    sleep
    nil
  end

  def self.critical
    @critical
  end

  def self.critical=(value)
    set_critical value
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

    if not @alive
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

  def set_debugging(dc, cc)
    raise "very unlikely to run"
  end

  def debug_channel
    raise "nope!"
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

  # Implementation note: ideally, the recursive_objects
  # lookup table would be different per method call.
  # Currently it doesn't cause problems, but if ever
  # a method :foo calls a method :bar which could 
  # recurse back to :foo, it could require making
  # the tables independant.

  def self.recursion_guard(obj)
    id = obj.object_id
    objects = current.recursive_objects
    objects[id] = true

    begin
      yield
    ensure
      objects.delete id
    end
  end

  def self.guarding?(obj)
    current.recursive_objects[obj.object_id]
  end

  # detect_recursion will return if there's a recursion
  # on obj (or the pair obj+paired_obj).
  # If there is one, it returns true.
  # Otherwise, it will yield once and return false.

  def self.detect_recursion(obj, paired_obj=undefined)
    id = obj.object_id
    pair_id = paired_obj.object_id
    objects = current.recursive_objects

    case objects[id]

      # Default case, we haven't seen +obj+ yet, so we add it and run the block.
    when nil
      objects[id] = pair_id
      begin
        yield
      ensure
        objects.delete id
      end

      # We've seen +obj+ before and it's got multiple paired objects associated
      # with it, so check the pair and yield if there is no recursion.
    when Rubinius::LookupTable
      return true if objects[id][pair_id]
      objects[id][pair_id] = true

      begin
        yield
      ensure
        objects[id].delete pair_id
      end

      # We've seen +obj+ with one paired object, so check the stored one for
      # recursion.
      #
      # This promotes the value to a LookupTable since there is another new paired
      # object.
    else
      previous = objects[id]
      return true if previous == pair_id

      objects[id] = Rubinius::LookupTable.new(previous => true, pair_id => true)

      begin
        yield
      ensure
        objects[id] = previous
      end
    end

    false
  end

  # Similar to detect_recursion, but will short circuit all inner recursion
  # levels (using a throw)

  class InnerRecursionDetected < Exception; end

  def self.detect_outermost_recursion(obj, paired_obj=undefined, &block)
    rec = current.recursive_objects

    if rec[:__detect_outermost_recursion__]
      if detect_recursion(obj, paired_obj, &block)
        raise InnerRecursionDetected.new
      end
      false
    else
      begin
        rec[:__detect_outermost_recursion__] = true

        begin
          detect_recursion(obj, paired_obj, &block)
        rescue InnerRecursionDetected
          return true
        end

        return nil
      ensure
        rec.delete :__detect_outermost_recursion__
      end
    end
  end

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

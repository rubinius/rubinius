#--
# Be very careful about calling raise in here! Thread has its own
# raise which, if you're calling raise, you probably don't want. Use
# Kernel.raise to call the proper raise.
#++
class Thread
  attr_reader :recursive_objects
  attr_reader :pid
  attr_reader :exception
  attr_reader :stack_size
  attr_reader :source

  def self.new(*args, **kw, &block)
    stack_size = Rubinius::Type.try_convert kw[:stack_size], Fixnum, :to_int

    thr = Rubinius.invoke_primitive :thread_s_new, args, stack_size, block, self

    Rubinius::VariableScope.of_sender.locked!

    unless thr.send :initialized?
      raise ThreadError, "Thread#initialize not called"
    end

    return thr
  end

  def self.start(*args, **kw, &block)
    raise ArgumentError.new("no block passed to Thread.start") unless block

    stack_size = Rubinius::Type.try_convert kw[:stack_size], Fixnum, :to_int

    Rubinius.invoke_primitive :thread_s_start, args, stack_size, block, self
  end

  class << self
    alias_method :fork, :start
  end

  def initialize(*args, &block)
    unless block
      Kernel.raise ThreadError, "no block passed to Thread#initialize"
    end

    if @initialized
      Kernel.raise ThreadError, "already initialized thread"
    end

    @args = args
    @block = block
    @initialized = true

    Thread.current.group.add self
  end

  private :initialize

  alias_method :__thread_initialize__, :initialize

  def initialized?
    @initialized
  end

  private :initialized?

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

  def self.count
    Rubinius.primitive :thread_count
    Kernel.raise PrimitiveFailure, "Thread.count primitive failed"
  end

  def self.stop
    sleep
    nil
  end

  def name
    Rubinius.primitive :thread_name
    Kernel.raise PrimitiveFailure, "Thread#name primitive failed"
  end

  def name=(name)
    return unless name

    thread_name = StringValue(name)
    unless thread_name.ascii_only?
      Kernel.raise ArgumentError, "name must be ASCII only"
    end

    Rubinius.invoke_primitive :thread_set_name, self, thread_name

    name
  end

  def fiber_list
    Rubinius.primitive :thread_fiber_list
    Kernel.raise PrimitiveFailure, "Thread.fiber_list primitive failed"
  end

  def wakeup
    Rubinius.primitive :thread_wakeup
    Kernel.raise PrimitiveFailure, "Thread#wakeup primitive failed"
  end

  def priority
    Rubinius.primitive :thread_get_priority
    Kernel.raise ThreadError, "Thread#priority primitive failed"
  end

  def priority=(val)
    Rubinius.primitive :thread_set_priority
    Kernel.raise TypeError, "priority must be a Fixnum" unless val.kind_of? Fixnum
    Kernel.raise ThreadError, "Thread#priority= primitive failed"
  end

  def __context__
    Rubinius.primitive :thread_context
    Kernel.raise PrimitiveFailure, "Thread#__context__ primitive failed"
  end

  def mri_backtrace
    Rubinius.primitive :thread_mri_backtrace
    Kernel.raise PrimitiveFailure, "Thread#mri_backtrace primitive failed"
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
    str = "#<#{self.class}:0x#{object_id.to_s(16)} id=#{@thread_id} pid=#{@pid}"
    str << " source=#{@source}" if @source
    str << " status=#{status || "dead"}"
    str << ">"
  end

  alias_method :to_s, :inspect

  def alive?
    Rubinius::Mirror.reflect(self).alive?
  end

  def stop?
    Rubinius::Mirror.reflect(self).stop?
  end

  def status
    Rubinius::Mirror.reflect(self).status
  end

  def join(timeout=undefined)
    if undefined.equal? timeout or nil.equal? timeout
      timeout = nil
    else
      timeout = Rubinius::Type.coerce_to_float timeout
    end

    value = Rubinius.invoke_primitive :thread_join, self, timeout

    if @exception
      Kernel.raise @exception
    else
      value
    end
  end

  def group
    @group
  end

  def raise(exc=undefined, msg=nil, trace=nil)
    return self unless alive?

    if exc.respond_to? :exception
      exc = exc.exception msg
      Kernel.raise TypeError, 'exception class/object expected' unless Exception === exc
      exc.set_backtrace trace if trace
    elsif undefined.equal? exc
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
      Rubinius.invoke_primitive :thread_raise, self, exc
    end
  end

  def thread_variable_get(key)
    key = Rubinius::Type.coerce_to_symbol key
    Rubinius.invoke_primitive :thread_variable_get, self, key
  end

  def thread_variable_set(key, value)
    key = Rubinius::Type.coerce_to_symbol key
    Rubinius.invoke_primitive :thread_variable_set, self, key, value
  end

  def thread_variable?(key)
    key = Rubinius::Type.coerce_to_symbol key
    Rubinius.invoke_primitive :thread_variable_key_p, self, key
  end

  def thread_variables
    Rubinius.primitive :thread_variables
    raise PrimitiveFailure, "Thread#thread_variables primitive failed"
  end

  def [](key)
    key = Rubinius::Type.coerce_to_symbol key
    Rubinius.invoke_primitive :thread_fiber_variable_get, self, key
  end

  def []=(key, value)
    key = Rubinius::Type.coerce_to_symbol key
    Rubinius.invoke_primitive :thread_fiber_variable_set, self, key, value
  end

  def key?(key)
    key = Rubinius::Type.coerce_to_symbol key
    Rubinius.invoke_primitive :thread_fiber_variable_key_p, self, key
  end

  def keys
    Rubinius.primitive :thread_fiber_variables
    raise PrimitiveFailure, "Thread#keys primitive failed"
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

  def kill
    Rubinius.invoke_primitive :thread_kill, self
  end

  alias_method :exit, :kill
  alias_method :terminate, :kill

  def value
    join
    @value
  end

  def self.exclusive
    MUTEX_FOR_THREAD_EXCLUSIVE.synchronize { yield }
  end

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
        raise InnerRecursionDetected
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

  def randomizer
    @randomizer ||= Rubinius::Randomizer.new
  end

  def backtrace
    mri_backtrace.map do |tup|
      code = tup[0]
      line = tup[1]
      is_block = tup[2]
      name = tup[3]

      "#{code.active_path}:#{line}:in `#{name}'"
    end
  end

  # This is a class in MRI, although you can't initialize it.
  class Backtrace
    # Stores location of a single call frame, available since Ruby 2.0.
    class Location
      attr_reader :label
      attr_reader :path
      attr_reader :absolute_path
      attr_reader :lineno

      def initialize(label, absolute_path, path, lineno)
        @label         = label
        @absolute_path = absolute_path
        @path          = path
        @lineno        = lineno
      end

      private :initialize

      alias_method :base_label, :label

      def to_s
        "#{absolute_path}:#{lineno}:in `#{label}'"
      end

      def inspect
        to_s
      end
    end
  end
end

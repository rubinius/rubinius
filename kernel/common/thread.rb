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

  def self.new(*args, &block)
    thr = allocate()
    thr.initialize *args, &block
    thr.fork

    return thr
  end

  def self.start(*args, &block)
    new(*args, &block) # HACK
  end

  def initialize(*args, &block)
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
        @result = @block.call *@args
      ensure
        @lock.receive
        @joins.each {|join| join.send self }
      end
    rescue Die
      @exception = nil
    rescue Exception => e
      @exception = e
    ensure
      @alive = false
      @lock.send nil
    end

    if @exception
      if Thread.abort_on_exception
        Thread.main.raise @exception
      elsif $DEBUG
        STDERR.puts "Exception in thread: #{@exception.message} (#{@exception.class})"
      end
    end
  end

  def setup(prime_lock)
    @group = nil
    @alive = true
    @result = nil
    @exception = nil
    @critical = false
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
        jc = Rubinius::Channel.new
        @joins << jc
        @lock.send nil
        begin
          if timeout.equal? Undefined
            jc.receive
          else
            jc.receive_timeout timeout.to_f
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
    raise "very unlikely to run"
  end

  def debug_channel
    raise "nope!"
  end

  attr_reader :control_channel

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
    Context.new *__context__
  end

  alias_method :current_context, :context
end

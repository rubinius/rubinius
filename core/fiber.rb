class Fiber
  attr_reader :stack_size
  attr_reader :thread_name
  attr_reader :fiber_id
  attr_reader :source

  def self.new(**kw, &block)
    if block.nil?
      raise ArgumentError, "Fiber.new requires a block"
    end

    stack_size = Rubinius::Type.try_convert kw[:stack_size], Fixnum, :to_int

    Rubinius.invoke_primitive :fiber_new, stack_size, block, self
  end

  def self.current
    Rubinius.primitive :fiber_s_current
    raise PrimitiveFailure, "Fiber.current primitive failed"
  end

  def self.yield(*args)
    Rubinius.primitive :fiber_s_yield
    raise PrimitiveFailure, "Fiber.yield primitive failed"
  end

  def self.list
    Rubinius.primitive :fiber_s_list
    raise PrimitiveFailure, "Fiber.list primitive failed"
  end

  def self.main
    Rubinius.primitive :fiber_s_main
    raise PrimitiveFailure, "Fiber.main primitive failed"
  end

  def self.count
    Rubinius.primitive :fiber_s_count
    raise PrimitiveFailure, "Fiber.count primitive failed"
  end

  def status
    Rubinius.primitive :fiber_status
    raise PrimitiveFailure, "Fiber#status primitive failed"
  end

  def resume(*args)
    Rubinius.primitive :fiber_resume
    raise PrimitiveFailure, "Fiber#resume primitive failed"
  end

  def transfer(*args)
    Rubinius.primitive :fiber_transfer
    raise PrimitiveFailure, "Fiber#transfer primitive failed"
  end

  def dispose
    Rubinius.primitive :fiber_dispose
    raise PrimitiveFailure, "Fiber#dispose primitive failed"
  end

  def alive?
    status != "dead"
  end

  def inspect
    str = "#<#{self.class}:0x#{object_id.to_s(16)} thread_name=#{@thread_name} fiber_id=#{@fiber_id} status=#{status}"
    str << " source=#{@source}" if @source
    str << ">"
  end

  alias_method :to_s, :inspect
end

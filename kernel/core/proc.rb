# depends on: class.rb context.rb binding.rb

class Proc
  self.instance_fields = 2
  ivar_as_index :__ivars__ => 0, :block => 1

  attr_writer :binding
  def block; @block ; end

  def block=(other)
    @block = other
  end

  def binding
    return @binding if @binding
    @binding = Binding.from_env @block
  end

  def caller(start = 0)
    @block.home_block.stack_trace_starting_at(0)
  end

  def self.__from_block__(env)
    if env.__kind_of__(BlockEnvironment)
      obj = allocate()
      env.last_ip = env.home_block.ip if env.home_block
      obj.block = env
      return obj
    else
      begin
        env.to_proc
      rescue Exception
        raise ArgumentError, "Unable to convert #{env.inspect} to a Proc"
      end
    end
  end

  def self.new
    if block_given?
      env = MethodContext.current.block
    else
      # Support for ancient pre-block-pass style:
      # def something; Proc.new; end
      # something { a_block } => Proc instance
      env = MethodContext.current.sender.block
    end

    if env
      return __from_block__(env)
    else
      raise ArgumentError, "tried to create a Proc object without a block"
    end
  end

  def inspect
    "#<#{self.class}:0x#{self.object_id.to_s(16)} @ #{self.block.file}:#{self.block.line}>"
  end

  alias_method :to_s, :inspect

  def ==(other)
    return false unless other.kind_of? self.class
    @block == other.block
  end

  def arity
    @block.arity
  end

  def to_proc
    self
  end

  def call(*args)
    @block.call(*args)
  end

  alias_method :[], :call

  class Function < Proc
    ivar_as_index :block => 1

    def call(*args)
      a = arity()
      unless a < 0 or a == 1 or args.size == a
        raise ArgumentError, "wrong number of arguments (#{args.size} for #{arity})"
      end

      begin
        @block.call(*args)
      rescue IllegalLongReturn, LongReturnException => e
        return e.value
      end
    end
    
    alias_method :[], :call
  end
end

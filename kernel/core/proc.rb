# depends on: class.rb

class Proc
  self.instance_fields = 3
  ivar_as_index :__ivars__ => 0, :block => 1, :check_args => 2

  def block; @block ; end

  def block=(other)
    @block = other
  end

  def check_args=(other)
    @check_args = other
  end

  # An optimized version, used for the &block syntax
  def self.__from_block__(env)
    if env.__kind_of__(BlockEnvironment)
      obj = allocate()
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

  def self.from_environment(env, check_args=false)
    if env.nil?
      nil
    elsif env.kind_of?(BlockEnvironment)
      obj = allocate()
      obj.block = env
      obj.check_args = check_args
      obj
    elsif env.respond_to? :to_proc
      env.to_proc
    else
      raise ArgumentError.new("Unable to turn a #{env.inspect} into a Proc")
    end
  end

  def self.new(&block)
    if block
      return block
    else
      # This behavior is stupid.
      be = MethodContext.current.sender.block
      if be
        return from_environment(be)
      else
        raise ArgumentError, "tried to create a Proc object without a block"
      end
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

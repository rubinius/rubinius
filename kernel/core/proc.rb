# depends on: class.rb context.rb binding.rb

class Proc
  self.instance_fields = 2
  ivar_as_index :__ivars__ => 0, :block => 1

  def block; @block ; end

  def block=(other)
    @block = other
  end

  def compiled_method; @compiled_method; end

  def compiled_method=(other)
    @compiled_method = other
  end

  def block_based?
    not block.nil?
  end

  def binding
    Binding.from_env @block if block_based?
  end

  def caller(start = 0)
    @block.home_block.stack_trace_starting_at(start) if block_based?
  end

  def self.__from_block__(env)
    if env.__kind_of__(BlockEnvironment)
      obj = allocate()
      obj.block = env
      obj.block.registration_ip = env.home_block.ip if env.home_block
      return obj
    else
      begin
        env.to_proc
      rescue Exception
        raise ArgumentError, "Unable to convert #{env.inspect} to a Proc"
      end
    end
  end

  def self.__from_compiled_method__(cm)
    obj = allocate()
    obj.compiled_method = cm
    obj.metaclass.method_table[:call] = obj.metaclass.method_table[:[]] = cm
    return obj
  end

  def self.new(compiled_method = nil)
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
    elsif compiled_method and compiled_method.__kind_of__(CompiledMethod)
      return __from_compiled_method__(compiled_method)
    else
      raise ArgumentError, "tried to create a Proc object without a block"
    end
  end

  def inspect
    if block_based?
      line = @block.home_block.line
      file = @block.home_block.file
    else
      line = @compiled_method.first_line if @compiled_method.lines
      file = @compiled_method.file
    end
    "#<#{self.class}:0x#{self.object_id.to_s(16)} @ #{file}:#{line}>"
  end

  alias_method :to_s, :inspect

  def ==(other)
    return false unless other.kind_of? self.class
    return @compiled_method == other.compiled_method unless block_based?
    @block == other.block
  end

  def arity
    unless block_based?
      return -1 if @compiled_method.kind_of? NativeMethod
      c = @compiled_method.args.inject(0) { |s, v| s += (v.length rescue 0) }
      return (@compiled_method.args[2].nil?) ? c : -(c + 1)
    end
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

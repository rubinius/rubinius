# depends on: class.rb binding.rb

class Proc

  def self.__from_block__(env)
    Ruby.primitive :block_wrapper_from_env

    if env.__kind_of__(BlockEnvironment)
      obj = FromBlock.allocate
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

  def self.new(compiled_method = nil)
    if compiled_method
      return Proc::CompiledMethod.new(compiled_method)
    elsif block_given?
      env = block_given?
    else
      # Support for ancient pre-block-pass style:
      # def something; Proc.new; end
      # something { a_block } => Proc instance
      env = VariableScope.of_sender.block
    end

    if env
      return __from_block__(env)
    else
      raise ArgumentError, "tried to create a Proc object without a block"
    end
  end

  attr_accessor :block

  def binding
    bind = Binding.setup @block.home_block
    bind.proc_environment = @block
    bind
  end

  def caller(start = 0)
    @block.home_block.stack_trace_starting_at(start)
  end


  def inspect
    "#<Proc:0x#{self.object_id.to_s(16)} @ #{@block.file}:#{@block.line}>"
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

  class FromBlock
    alias_method :[], :call
  end

  class Function < Proc
    def self.__setup__(block)
      obj = allocate()
      obj.block = block
      return obj
    end

    def call(*args)
      a = arity()
      unless a < 0 or a == 1 or args.size == a
        raise ArgumentError, "wrong number of arguments (#{args.size} for #{arity})"
      end

      @block.call(*args)
    end

    alias_method :[], :call
  end

  class CompiledMethod < Proc
    def compiled_method; @compiled_method; end

    def compiled_method=(other)
      @compiled_method = other
    end

    def self.__from_compiled_method__(cm)
      obj = allocate()
      obj.compiled_method = cm

      # This bears explaining.
      # We have a CompiledMethod object, so what we do is just make it
      # our call method. We do this by putting it the method into our
      # metaclass's MethodTable.
      #
      # This way, when someone sends us call or [], the method is automatically
      # called.
      obj.metaclass.method_table[:call] = cm
      obj.metaclass.method_table[:[]]   = cm
      return obj
    end

    def self.new(cm)
      if cm.kind_of? ::CompiledMethod
        return __from_compiled_method__(cm)
      else
        raise ArgumentError, "tried to create a CompiledMethodProc object without a CompiledMethod"
      end
    end

    def inspect
      line = @compiled_method.first_line if @compiled_method.lines
      file = @compiled_method.file
      "#<#{self.class}:0x#{self.object_id.to_s(16)} @ #{file}:#{line}>"
    end

    alias_method :to_s, :inspect

    def ==(other)
      return false unless other.kind_of? self.class
      @compiled_method == other.compiled_method
    end

    def arity
      return -1 if @compiled_method.kind_of? NativeMethod
      c = @compiled_method.args.inject(0) { |s, v| s += (v.length rescue 0) }
      return (@compiled_method.args[2].nil?) ? c : -(c + 1)
    end
  end
end

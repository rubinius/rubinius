class Proc

  def self.__from_block__(env)
    Ruby.primitive :proc_from_env

    if env.__kind_of__(Rubinius::BlockEnvironment)
      raise PrimitiveFailure, "Unable to create Proc from BlockEnvironment"
    else
      begin
        env.to_proc
      rescue Exception
        raise ArgumentError, "Unable to convert #{env.inspect} to a Proc"
      end
    end
  end

  # This works because the VM implements &block using push_proc, which
  # creates a Proc inside the VM.
  def self.new(method=nil, &block)
    if method
      return Proc::Method.new(method)
    elsif block
      return block
    else
      # Support for ancient pre-block-pass style:
      # def something; Proc.new; end
      # something { a_block } => Proc instance
      env = Rubinius::VariableScope.of_sender.block
    end

    if env
      return __from_block__(env)
    else
      raise ArgumentError, "tried to create a Proc object without a block"
    end
  end

  attr_accessor :block

  def binding
    bind = Binding.setup @block.scope, @block.method, @block.method.scope
    bind.proc_environment = @block
    bind
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

  alias_method :[], :call
  alias_method :yield, :call

  class Method < Proc
    def bound_method
      @bound_method
    end

    def bound_method=(other)
      @bound_method = other
    end

    def self.__from_method__(meth)
      obj = allocate()
      obj.bound_method = meth

      return obj
    end

    def __yield__(*args, &block)
      # do a block style unwrap..
      if args.size == 1 and args.first.kind_of? Array
        args = args.first
      end

      @bound_method.call(*args, &block)
    end

    def call(*args, &block)
      @bound_method.call(*args, &block)
    end
    alias_method :[], :call

    def self.new(meth)
      if meth.kind_of? ::Method
        return __from_method__(meth)
      else
        raise ArgumentError, "tried to create a Proc::Method object without a Method"
      end
    end

    def inspect
      cm = @bound_method.executable
      if cm.respond_to? :file
        if cm.lines
          line = cm.first_line
        else
          line = "-1"
        end
        file = cm.file
      else
        line = "-1"
        file = "(unknown)"
      end

      "#<#{self.class}:0x#{self.object_id.to_s(16)} @ #{file}:#{line}>"
    end

    alias_method :to_s, :inspect

    def ==(other)
      return false unless other.kind_of? self.class
      @bound_method == other.bound_method
    end

    def arity
      @bound_method.arity
    end
  end
end

class Proc

  def self.__from_block__(env)
    Ruby.primitive :proc_from_env

    if Rubinius::Type.object_kind_of? env, Rubinius::BlockEnvironment
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
  def self.new(*args)
    env = nil

    Rubinius.asm do
      push_block
      set_local 1
    end

    unless env
      # Support for ancient pre-block-pass style:
      # def something; Proc.new; end
      # something { a_block } => Proc instance
      env = Rubinius::BlockEnvironment.of_sender

      unless env
        raise ArgumentError, "tried to create a Proc object without a block"
      end
    end

    block = __from_block__(env)

    Rubinius.asm(block, args) do |b,a|
      run b
      run a
      run b
      send_with_splat :initialize, 0, true
    end

    return block
  end

  # Expose @block because MRI does. Do not expose @bound_method
  attr_accessor :block

  def binding
    bind = @block.to_binding
    bind.proc_environment = @block
    bind
  end

  def inspect
    "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{@block.file}:#{@block.line}>"
  end

  alias_method :to_s, :inspect

  def ==(other)
    return false unless other.kind_of? self.class
    @block == other.block
  end

  def arity
    if @bound_method
      arity = @bound_method.arity
      return arity < 0 ? -1 : arity
    end

    @block.arity
  end

  def to_proc
    self
  end

  alias_method :[], :call
  alias_method :yield, :call

  class Method < Proc
    attr_accessor :bound_method

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

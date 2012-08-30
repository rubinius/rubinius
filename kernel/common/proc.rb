# -*- encoding: us-ascii -*-

class Proc

  def self.__from_block__(env)
    Rubinius.primitive :proc_from_env

    if Rubinius::Type.object_kind_of? env, Rubinius::BlockEnvironment
      raise PrimitiveFailure, "Proc.__from_block__ primitive failed to create Proc from BlockEnvironment"
    else
      begin
        env.to_proc
      rescue Exception
        raise ArgumentError, "Unable to convert #{env.inspect} to a Proc"
      end
    end
  end

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

    Rubinius.asm(block, args) do |b, a|
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

  def parameters
    if @bound_method
      return @bound_method.parameters
    end

    code = @block.compiled_code

    return [] unless code.respond_to? :local_names

    m = code.required_args - code.post_args
    o = m + code.total_args - code.required_args
    p = o + code.post_args
    p += 1 if code.splat

    required_status = self.lambda? ? :req : :opt

    code.local_names.each_with_index.map do |name, i|
      if i < m
        [required_status, name]
      elsif i < o
        [:opt, name]
      elsif code.splat == i
        name == :@unnamed_splat ? [:rest] : [:rest, name]
      elsif i < p
        [required_status, name]
      else
        [:block, name]
      end
    end
  end


  def to_proc
    self
  end

  alias_method :[], :call
  alias_method :yield, :call

  def clone
    copy = self.class.__allocate__
    Rubinius.invoke_primitive :object_copy_object, copy, self
    Rubinius.invoke_primitive :object_copy_singleton_class, copy, self

    Rubinius.privately do
      copy.initialize_copy self
    end

    copy.freeze if frozen?
    copy
  end

  def dup
    copy = self.class.__allocate__
    Rubinius.invoke_primitive :object_copy_object, copy, self

    Rubinius.privately do
      copy.initialize_copy self
    end
    copy
  end

  class Method < Proc
    attr_accessor :bound_method

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
      code = @bound_method.executable
      if code.respond_to? :file
        if code.lines
          line = code.first_line
        else
          line = "-1"
        end
        file = code.file
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

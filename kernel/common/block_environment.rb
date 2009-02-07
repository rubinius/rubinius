##
# Describes the environment a block was created in.  BlockEnvironment is used
# to create a BlockContext.

class BlockEnvironment
  attr_accessor :local_count
  attr_accessor :method # The CompiledMethod object that we were called from

  attr_accessor :initial_ip
  attr_accessor :last_ip
  attr_accessor :post_send
  attr_accessor :scope

  attr_accessor :proc_environment
  attr_accessor :metadata_container
  attr_writer   :constant_scope # Static scope for constant lookup

  def from_proc?
    @proc_environment
  end

  def under_context(context, cmethod)
    if context.__kind_of__ BlockContext
      home = context.home
    else
      home = context
    end

    @home = home
    @home_block = context
    @method = cmethod
    @local_count = cmethod.local_count

    @initial_ip = 0
    @last_ip = 0x10000000 # 2**28
    @post_send = 0
    return self
  end

  ##
  # First field of the tuple in @metadata_container holds a boolean
  # indicating if the context is from eval

  def from_eval?
    @metadata_container and @metadata_container[0]
  end

  def from_eval!
    @metadata_container = Tuple.new(1) unless @metadata_container
    @metadata_container[0] = true
  end

  def make_independent
    @home = @home.dup
    # TODO: enabling this appears to break Module.module_function, why?
    # @home_block = @home_block.dup
    @method = @method.dup
  end

  def redirect_to(obj)
    env = dup
    env.make_independent
    env.home.receiver = obj
    return env
  end

  def call_on_instance(obj, *args)
    obj = redirect_to(obj)
    obj.call(*args)
  end

  def arity
    method.required_args
  end

  def constant_scope
    @constant_scope ||= @method.scope
  end

  def disable_long_return!
    @post_send = nil
  end

  # TODO: are file,line actually used?
  def file
    method.file
  end

  def line
    method.line_from_ip(0)
  end
end

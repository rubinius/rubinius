# TODO - Needs ivar_as_index removal cleanup

##
# Describes the environment a block was created in.  BlockEnvironment is used
# to create a BlockContext.

class BlockEnvironment

  attr_reader :home
  attr_reader :home_block
  attr_reader :local_count
  attr_reader :method

  attr_accessor :proc_environment
  
  def from_proc?
    @proc_environment
  end

  def home_block=(block)
    @home_block = block
  end

  def initial_ip=(ip)
  end

  def last_ip=(ip)
  end

  def metadata_container
    @metadata_container
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
  # Holds a Tuple of additional metadata.
  # First field of the tuple holds a boolean indicating if the context is from
  # eval
  def metadata_container=(tup)
    @metadata_container = tup
  end

  def from_eval?
    @metadata_container and @metadata_container[0]
  end

  def from_eval!
    @metadata_container = Tuple.new(1) unless @metadata_container
    @metadata_container[0] = true
  end

  ##
  # The CompiledMethod object that we were called from

  def method=(tup)
    @method = tup
  end

  def home=(home)
    @home = home
  end

  def scope=(tup)
    @scope = tup
  end

  def make_independent
    @home = @home.dup
    @home_block = @home_block.dup
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

  # Static scope for constant lookup
  def constant_scope=(scope)
    @constant_scope = scope
  end

  def constant_scope
    @constant_scope ||= @method.scope
  end
end


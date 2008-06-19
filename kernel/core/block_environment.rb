
##
# Describes the environment a block was created in.  BlockEnvironment is used
# to create a BlockContext.

class BlockEnvironment
  ivar_as_index :__ivars__ => 0, :home => 1, :initial_ip => 2, :last_ip => 3,
    :post_send => 4, :home_block => 5, :local_count => 6, :metadata_container => 7, :method => 8
  def __ivars__   ; @__ivars__   ; end
  def home        ; @home        ; end
  def initial_ip  ; @initial_ip  ; end
  def last_ip     ; @last_ip     ; end
  def post_send   ; @post_send   ; end
  def home_block  ; @home_block  ; end
  def local_count ; @local_count ; end
  def method      ; @method      ; end

  attr_accessor :proc_environment
  def from_proc?
    @proc_environment
  end

  def home_block=(block)
    @home_block = block
  end

  def initial_ip=(ip)
    @initial_ip = ip
  end

  def last_ip=(ip)
    @last_ip = ip
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

  def disable_long_return!
    @post_send = nil
  end

  def arity
    method.required
  end

  # Static scope for constant lookup
  def constant_scope=(scope)
    @constant_scope = scope
  end

  def constant_scope
    @constant_scope ||= @method.staticscope
  end
end


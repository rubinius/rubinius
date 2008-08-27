# depends on: class.rb

##
# Stores all information about a running Block.

class BlockContext

  def last_match
    home.last_match
  end

  def last_match=(match)
    home.last_match = match
  end

  def nth_ref(idx)
    home.nth_ref(idx)
  end

  def back_ref(idx)
    home.back_ref(idx)
  end

  def home
    env.home
  end

  def name
    home.name
  end

  def receiver
    home.receiver
  end

  def method_module
    home.method_module
  end

  def current_scope
    home.current_scope
  end
end

##
# Describes the environment a Block was created in.  BlockEnvironment is used
# to create a BlockContext.

class BlockEnvironment
  def __ivars__   ; @__ivars__   ; end
  def home        ; @home        ; end
  def initial_ip  ; @initial_ip  ; end
  def last_ip     ; @last_ip     ; end
  def post_send   ; @post_send   ; end
  def home_block  ; @home_block  ; end
  def local_count ; @local_count ; end
  def bonus       ; @bonus       ; end
  def method      ; @method      ; end

  def under_context(home, cmethod)
    if home.kind_of? BlockContext
      home_block = home
      home = home.home
    else
      home_block = home
    end

    @home = home
    @initial_ip = 0
    @last_ip = 0x10000000 # 2**28
    @post_send = 0
    @home_block = home_block
    @method = cmethod
    @local_count = cmethod.local_count
    return self
  end

  ##
  # Holds a Tuple of local variable names to support eval

  def bonus=(tup)
    @bonus = tup
  end

  def from_eval?
    @bonus and @bonus[0]
  end

  def from_eval!
    @bonus = Tuple.new(1) unless @bonus
    @bonus[0] = true
  end

  ##
  # The CompiledMethod object that we were called from

  def method=(tup)
    @method = tup
  end

  ##
  #--
  # These should be safe since I'm unsure how you'd have a BlockContext
  # and have a nil CompiledMethod (something that can (and has) happened
  # with MethodContexts)

  def file
    method.file
  end

  def line
    method.line_from_ip(initial_ip)
  end

  def home=(home)
    @home = home
  end

  def scope=(tup)
    @scope = tup
  end

  def make_independent
    @home = @home.dup
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
end


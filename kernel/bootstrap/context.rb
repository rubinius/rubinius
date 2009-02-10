class MethodContext
  def self.new
    raise TypeError, "MethodContext doesn't support #new"
  end

  def self.current
    cur = Rubinius.asm { push_context }
    return cur.sender
  end

  def activate(val)
    Ruby.primitive :activate_context
    raise PrimitiveFailure, "MethodContext#activate primitive failed"
  end

  def sender
    @sender
  end

  def ip
    _get_field(1)
  end

  def native_ip
    _get_field(3)
  end

  def jit_compiled?
    !!native_ip
  end

  def sp
    _get_field(2)
  end

  def block
    @block
  end

  def method
    @cm
  end

  def method=(obj)
    @cm = obj # NOTE SUPER DANGEROUS
  end

  def receiver
    @self
  end

  def receiver=(val)
    @self = val # NOTE SUPER DANGEROUS
  end

  def locals
    Ruby.primitive :context_locals
    raise PrimitiveFailure, "MethodContext#locals primitive failed"
  end

  def name
    @name or @cm.name
  end

  def method_module
    @module
  end

  def dup
    Ruby.primitive :context_dup
    raise PrimitiveFailure, "MethodContext#dup primitive failed"
  end

  def _get_field(int)
    Ruby.primitive :context_get_field
    raise PrimitiveFailure, "MethodContext#_get_field primitive failed"
  end
end

class BlockContext
  def env
    Ruby.primitive :block_context_env
    raise PrimitiveFailure, 'BlockContext#env primitive failed'
  end
end

class BlockEnvironment
  def self.allocate
    Ruby.primitive :blockenvironment_allocate
    raise PrimitiveFailure, "BlockEnvironment.allocate primitive failed"
  end

  def call(*args)
    Ruby.primitive :block_call
    raise PrimitiveFailure, "BlockEnvironment#call primitive failed"
  end
end

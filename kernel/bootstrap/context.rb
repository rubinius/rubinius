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
    raise PrimitiveFailure, "primitive failed"
  end

  def sender
    @sender
  end

  def ip
    _get_field(1)
  end

  def ip=(num)
    _set_field(1, num.to_i)
  end

  def sp
    _get_field(2)
  end

  def fp
    raise NotImplementedError, "gone in VM"
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
    _get_field(8)
  end

  def locals=(val)
    _set_field(8, val)
  end

  def name
    @name or @cm.name
  end

  def method_module
    @module
  end

  def dup
    Ruby.primitive :context_dup
    raise PrimitiveFailure, "primitive failed"
  end

  def _get_field(int)
    Ruby.primitive :context_get_field
    raise PrimitiveFailure, "primitive failed"
  end

  def _set_field(int, val)
    Ruby.primitive :context_set_field
    raise PrimitiveFailure, "primitive failed"
  end

  # Reloads the compiled method instruction sequence into the method context.
  # Required so that the debugger can add breakpoints to a currently executing
  # method context.
  def reload_method
    Ruby.primitive :context_reload_method
    raise PrimitiveFailure, "primitive failed"
  end

  # Modifies the context to use the specified instruction sequence.
  # This method exists to support debugging; it should only be used to modify
  # the task instruction sequence to add/remove temporary breakpoints that are
  # only for the current thread, e.g. when stepping or restoring a persistent
  # breakpoint.
  def set_iseq(iseq)
    Ruby.primitive :context_set_iseq
    raise PrimitiveFailure, "primitive failed"
  end
end

class BlockContext
  def env
    _get_field(10)
  end
end

class BlockEnvironment
  def call(*args)
    Ruby.primitive :block_call
    raise PrimitiveFailure, "primitive failed"
  end
end

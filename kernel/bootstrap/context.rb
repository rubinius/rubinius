class MethodContext
  def self.current
    cur = Rubinius.asm { push_context }
    return cur.sender
  end
    
  def activate(val)
    Ruby.primitive :activate_context    
  end
  
  def sender
    _get_field(0)
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
    _get_field(13)
  end

  def block
    _get_field(3)
  end
  
  def method
    _get_field(5)
  end
  
  def receiver
    _get_field(7)
  end
  
  def receiver=(val)
    _set_field(7, val)
  end

  def locals
    _get_field(8)
  end

  def locals=(val)
    _set_field(8, val)
  end

  def name
    _get_field(10)
  end

  def method_module
    _get_field(11)
  end
  
  def dup
    Ruby.primitive :fastctx_dup
  end
    
  def _get_field(int)
    Ruby.primitive :fastctx_get_field
  end
  
  def _set_field(int, val)
    Ruby.primitive :fastctx_set_field
  end

  # Reloads the compiled method instruction sequence into the method context.
  # Required so that the debugger can add breakpoints to a currently executing
  # method context.
  def reload_method
    Ruby.primitive :fastctx_reload_method
  end

  # Modifies the context to use the specified instruction sequence.
  # This method exists to support debugging; it should only be used to modify
  # the task instruction sequence to add/remove temporary breakpoints that are
  # only for the current thread, e.g. when stepping or restoring a persistent
  # breakpoint.
  def set_iseq(iseq)
    Ruby.primitive :fastctx_set_iseq
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
  end
end

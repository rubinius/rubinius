class MethodContext
  def self.current
    cur = Ruby.asm "push_context\n"
    return cur.sender
  end
  
  def sender
    Ruby.primitive :context_sender
    # We use a primitive here so that the underside is aware that
    # we're pulling a methodcontext into the normal fraw. Currently
    # this means that it can't be reused automatically.
  end
  
  def activate(val)
    Ruby.primitive :activate_context    
  end
  
end

class FastMethodContext
  
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
end

class BlockContext
  def activate(val)
    Ruby.primitive :activate_context    
  end
end

class BlockEnvironment
  def execute(args)
    Ruby.primitive :block_call
  end
end
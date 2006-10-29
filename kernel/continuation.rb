class Continuation
  def initialize(ctx)
    @method = ctx
    @method = copy_method(true)
  end
  
  def copy_method(locals=false)
    top = @method.copy(locals)
    ctx = top
    sender = ctx.sender
    while sender
      # p ctx.locals
      dp = sender.copy(locals)
      # dp.put 10, dp.locals.dup
      # p dp.locals
      ctx.put 1, dp
      ctx = dp
      sender = ctx.sender
    end
    
    return top
  end
  
  def extract_start(blk)
    @start = blk.block.post_send
  end
  
  def call
    ctx = copy_method()
    ctx.put 2, @start
    ctx.activate(nil)
  end
end

module Kernel
  def callcc(&blk)
    cur = MethodContext.current
    ctx = cur.sender
    cont = Continuation.new(ctx)
    cont.extract_start(blk)
    blk.call(cont)
    return nil
  end
end
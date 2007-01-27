class Continuation
  def initialize(method_context)
    @method = method_context
    @method = copy_method(true)
  end
  
  def copy_method(locals=false)
    top = @method.copy(locals)
    method_context = top
    sender = method_context.sender
    while sender
      # p method_context.locals
      dp = sender.copy(locals)
      # dp.put 10, dp.locals.dup
      # p dp.locals
      method_context.put 1, dp
      method_context = dp
      sender = method_context.sender
    end
    
    return top
  end
  
  def extract_start(block)
    @start = block.block.post_send
  end
  
  def call
    method_context = copy_method()
    method_context.put 2, @start
    method_context.activate(nil)
  end

  alias :[] :call
end

module Kernel
  def callcc(&block)
    current_context = MethodContext.current
    method_context = current_context.sender
    cont = Continuation.new(method_context)
    cont.extract_start(block)
    block.call(cont)
    return nil
  end
end

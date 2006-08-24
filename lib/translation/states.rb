class RsLocalState
  def initialize
    reset
  end
  
  def reset
    @locals = [:_, :~]
  end
  
  attr_accessor :locals
  
  def local(name)
    cnt = @locals.index(name)
    return cnt if cnt
    cnt = @locals.size
    @locals << name
    return cnt
  end
end
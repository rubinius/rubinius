class RsLocalState
  def initialize
    reset
  end
  
  def reset
    @locals = [:_, :~]
    @unique = 0
  end
  
  def unique_id
    @unique += 1
  end
  
  attr_accessor :locals, :unique
  
  def local(name)
    cnt = @locals.index(name)
    return cnt if cnt
    cnt = @locals.size
    @locals << name
    return cnt
  end
end
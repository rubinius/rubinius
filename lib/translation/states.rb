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
    unless cnt
      cnt = @locals.size
      @locals << name
    end
    
    return cnt
  end
end
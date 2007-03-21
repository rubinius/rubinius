class RsLocalState
  def initialize
    reset
  end
  
  def reset
    @locals = [:_, :~]
    @unique = 0
    @args = []
    @arg_splat = nil
  end
  
  def unique_id
    @unique += 1
  end
  
  attr_accessor :locals, :unique, :args, :arg_splat
  
  def local(name)
    cnt = @locals.index(name)
    unless cnt
      cnt = @locals.size
      @locals << name
    end
    
    return cnt
  end
  
  def local?(name)
    @locals.include? name
  end
end
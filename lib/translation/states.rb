class RsLocalState
  def initialize
    reset
  end
  
  def reset
    @locals = [:_, :~]
    @unique = 0
    @args = []
    @arg_splat = nil
    
    @top_scope = { :_ => 0, :~ => 1 }
    @current_scope = @top_scope
    @scopes = [@top_scope]
    @all_scopes = [@top_scope]
    @lslot = 1
  end
  
  def unique_id
    @unique += 1
  end
  
  attr_accessor :locals, :unique, :args, :arg_splat
  
  def old_local(name)
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
  
  def next_slot
    @lslot += 1
  end
  
  def number_of_locals
    @lslot + 1
  end
  
  # For name, locate it's allocated slot.
  def find_local(name, allocate=true)
    i = 0
    @scopes.each do |sc|
      if cnt = sc[name]
        # puts "Found #{name} depth #{i} => #{cnt}"
        return cnt
      end
      i += 1
    end
    
    if allocate
      slt = (@current_scope[name] = next_slot())
      # puts "Added #{name} at #{slt}"
      return slt
    else
      nil
    end
  end
  
  alias :local :find_local
  
  def new_scope
    # puts "entering new scope.. (#{@current_scope.inspect})"
    scope = {}
    @scopes.unshift scope
    @current_scope = scope
    @all_scopes << scope
    
    begin
      out = yield
    ensure
      old = @scopes.shift
      @current_scope = @scopes[0]
      # puts "exitting scope.. (#{old.inspect} / #{@current_scope.inspect})"
    end
    
    return out
  end
end
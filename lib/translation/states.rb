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
    @depth = 0
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
    # @lslot += 1
    @current_scope.size
  end
  
  def number_of_locals
    # @lslot + 1
    @top_scope.size
  end
  
  # For name, locate it's allocated slot.
  def find_local(name, allocate=true)
    i = 0
    
    # An optimization to say that the var can be placed in the only 
    # present scope.
    
    if @depth == 0 and @scopes.size == 1
      # We calculated it in a previous run.
      if slt = @current_scope[name]
        return slt
      end
      
      return nil unless allocate
      
      slt = (@current_scope[name] = next_slot())
      return slt
    end
    
    @scopes.each do |sc|
      if cnt = sc[name]
        # puts "Found #{name} depth #{i} => #{cnt}"
        # Optimization: The top scope is accessed with a seperate instruction
        # so be sure to emit that fact.
        if sc == @top_scope
          return cnt
        end
        
        return [i, cnt]
      end
      i += 1
    end
    
    if allocate
      slt = (@current_scope[name] = next_slot())
      # puts "Added #{name} at #{slt}"
      return [0, slt]
    end
    
    return nil
  end
  
  alias :local :find_local
  
  def new_scope
    # puts "entering new scope.. (#{@current_scope.inspect})"
    scope = {}
    @scopes.unshift scope
    @current_scope = scope
    @all_scopes << scope
    @depth += 1
    begin
      yield
    ensure
      old = @scopes.shift
      @current_scope = @scopes[0]
      @depth -= 1
      # puts "exitting scope.. (#{old.inspect} / #{@current_scope.inspect})"
    end
    
    return old.size
  end

  # 'for' uses this, since it needs to call :iter, but doesn't introduce a new scope
  def last_scope
    @all_scopes.last
  end
end

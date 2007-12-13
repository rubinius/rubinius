module Compiler1
  
class RsLocalState
  class LocalVariable
    def initialize(name)
      @name = name
      @sexps = []
      @assigned = false
      @value = nil
      @argument_pos = nil
      @slot = nil
      @alloca = nil
      @scope = nil
      @on_stack = true
      @has_default = false
      @magic = nil
      @called_methods = []
      @passed_as_argument = false
      @omit = true
      @min_depth = nil
    end
    
    attr_accessor :slot, :alloca, :scope, :has_default, :magic
    attr_reader :called_methods, :min_depth, :name
    
    def assigned!
      @assigned = true
      if @on_stack and argument?
        @on_stack = false
      end
      @omit = false
    end
    
    def set_depth(depth)
      if !@min_depth or depth < @min_depth
        @min_depth = depth
      end
    end
    
    def assigned?
      @assigned
    end
    
    def argument!(pos)
      @argument_pos = pos
    end
    
    def argument?
      !@argument_pos.nil?
    end
    
    def argument_position
      @argument_pos
    end
    
    def passed_as_argument!
      @passed_as_argument = true
    end
    
    def passed_as_argument?
      @passed_as_argument
    end
    
    def referenced!
      @omit = false
    end
    
    def omit?
      @omit
    end
    
    def stack_position
      @argument_pos
    end
    
    def attach(sexp)
      @sexps << sexp
      return sexp
    end
    
    def set_value(val)
      @sexps.each do |sx|
        sx.map! { |e| e == self ? val : e }
      end
      @value = val
    end
    
    def value?
      !@value.nil?
    end
    
    def value
      @value
    end
    
    def access_in_block!
      @on_stack = false
    end
    
    def disallow_stack_access!
      @on_stack = false
    end
    
    def on_stack?
      @on_stack
    end
       
    def inspect
      "#<LocalVariable name=#{@name} #{@value.inspect}/#{@slot.inspect}/#{@alloca.inspect} depth=#{@min_depth}>"
    end
    
    def access_assembly(idx=nil)
      if argument? and on_stack?
        "from_fp #{stack_position} ; arg #{@name}"
      elsif on_stack?
        "get_local_fp #{alloca} ; local #{@name}"
      elsif scope == :block
        raise "No location index given" unless idx
        "push #{@name}:#{idx[0]}:#{idx[1]}"
      else
        "push #{@name}:#{slot}"
      end
    end
    
    def set_assembly(idx=nil)
      if scope == :block
        if idx.nil?
          raise "Invalid local variable detected. #{@name} is a block var, but has no index."
        end
        
        "set #{@name}:#{idx[0]}:#{idx[1]}"
      elsif on_stack?
        "set_local_fp #{alloca} ; local #{@name}"
      else
        "set #{@name}:#{slot}"
      end
    end
    
  end

  def initialize
    reset
  end
  
  def reset
    @unique = 0
    @args = []
    @arg_splat = nil
    @arg_block = nil
    
    @top_scope = {}
    @current_scope = @top_scope
    @scopes = [@top_scope]
    @all_scopes = [@top_scope]
    @lslot = 1
    @depth = 0
    @nostack_access = []
    @block_access = []
    @uses_eval = false
    
    @alloca = 1
    @alloca_locals = {}
    @locals = {}
  end
  
  attr_reader :arg_splat, :arg_block, :locals
  attr_accessor :uses_eval
  
  def each_arg
    @args.each do |arg|
      yield arg[0], arg[1]
    end
  end
  
  def collapse_lvars!
    each_arg do |name, lv|
      if lv.assigned? or !lv.on_stack?
        lv.slot = find_top_local(name)
      end
    end
    
    @locals.keys.each do |name|
      lv = @locals[name]

      unless lv.argument?
        if lv.on_stack?
          lv.alloca = alloca_local(name)
        else
          lv.slot = find_top_local(name)
        end
      end
    end
  end
  
  def local(name, allocate=true)
    if allocate
      lv = @locals[name] || LocalVariable.new(name)
      lv.disallow_stack_access! if @uses_eval
      @locals[name] = lv
    else
      @locals[name]
    end
  end
  
  def unique_id
    @unique += 1
  end
  
  attr_accessor :unique, :args
  
  def alloca_local(name)
    if pos = @alloca_locals[name]
      return pos
    end
    
    loc = @alloca
    @alloca_locals[name] = loc
    @alloca += 1
    return loc
  end
  
  def add_arg(name)
    lv = local(name)
    lv.scope = :normal
    lv.argument!(@args.size)
    
    @args << [name, lv]
    return lv
  end
  
  def arg?(name)
    @args.assoc(name)
  end
  
  def arg_position(name)
    i = 0
    @args.each { |v| return i if v[0] == name; i += 1 }
    return nil
  end
  
  def number_of_arguments
    @args.size
  end
  
  def arg_splat=(name)
    lv = local(name)
    lv.scope = :normal
    lv.magic = :splat
    @arg_splat = [name, lv]
  end
    
  def arg_block=(name)
    lv = local(name)
    lv.scope = :normal
    lv.magic = :block
    @arg_block = [name, lv]
  end
    
  def next_slot
    # @lslot += 1
    @current_scope.size
  end
  
  def number_of_locals
    # @lslot + 1
    @top_scope.size
  end
  
  def normal_lvar?(name)
    !@top_scope[name].nil?
  end
  
  def local_exists?(name)
    @locals.key?(name)
  end
  
  # How much space on the stack should we allocate for locals
  def stack_space
    @alloca - 1
  end
  
  def disallow_stack_access(name)
    @nostack_access << name unless @nostack_access.index(name)
  end
  
  def no_stack_access?(name)
    @nostack_access.index(name)
  end
  
  def stack_access?(name)
    !@nostack_access.index(name)
  end
  
  def checking_arg_assignment(name)
    if @args.assoc(name)
      @nostack_access << name
    end
  end
  
  def accessed_in_block(name)
    @block_access << name
  end
  
  def positional_arg?(name)
    @args.assoc(name) and !@nostack_access.index(name)
  end
  
  def find_top_local(name, allocate=true, assign=false)
      
    checking_arg_assignment(name) if assign
    
    # We calculated it in a previous run.
    if slt = @top_scope[name]
      # puts "Found #{name} in slot #{slt}"
      return slt
    end
    
    return nil unless allocate
    
    slt = (@top_scope[name] = next_slot())
    return slt
  end
  
  # For name, locate it's allocated slot.
  def find_local(name, allocate=true, assign=false)
    i = 0
    
    # An optimization to say that the var can be placed in the only 
    # present scope.
    
    if @depth == 0 and @scopes.size == 1
      return find_top_local(name, allocate, assign)
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
      checking_arg_assignment(name) if assign
      slt = (@current_scope[name] = next_slot())
      
      # puts "Added #{name} at #{slt}"
      return [0, slt]
    end
    
    return nil
  end
  
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
      this_scope = @scopes.shift
      @current_scope = @scopes[0]
      @depth -= 1
    end
    
    t = Tuple.new(this_scope.size)
    this_scope.each do |k,v|
      t[v] = k
    end
    t
  end

  # 'for' uses this, since it needs to call :iter, 
  # but doesn't introduce a new scope for its 'block variable'
  def surrounding_scope
    @scopes.first
  end

  def slot_locals_tuple
    top_locals = []
    max_index = 0
    @locals.each do |k,v| 
      if v.slot and v.min_depth == 0
        top_locals << v 
        max_index = v.slot if v.slot > max_index
      end
    end
    return nil if top_locals.empty?
    tuple = Tuple.new(max_index + 1)
    top_locals.each do |l|
      tuple[l.slot] = l.name
    end
    tuple
  end

  def stack_locals_tuple
    top_locals = []
    max_index = 0
    @locals.each do |k,v| 
      if v.alloca and v.min_depth == 0
        top_locals << v 
        max_index = v.alloca if v.alloca > max_index
      end
    end
    return nil if top_locals.empty?
    tuple = Tuple.new(max_index + 1)
    top_locals.each do |l|
      tuple[l.alloca] = l.name
    end
    tuple
  end
end

end
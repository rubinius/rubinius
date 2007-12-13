require 'compiler1/sexp/simple_processor'
require 'compiler1/translation/states'

# This converts a ruby 1.8.x sexp's idea of the scoping of 
# local variables into rubinius 1.0's idea.
# 
# Local variables are scoped the same as 1.8, using the depth
# + slot addressing protocol YARV also uses. 
#
# NOTE: I quite dislike that a block arg can be a global or an
# instance variable. This behavior is totally unexpected and 
# the syntax for it is pretty darn ugly. I'd love to get rid
# of it.
# Grepping through the 1.8 stdlib I actually can't find a single
# place where globals or instance/class vars are used as block
# args. All the more reason to remove it.
# ARG. I looked at parse.y and found that there are an additional
# 4 syntaxes also allowed as block arguments for assigned to 
# at "attribute". Again, I've never seen them used and believe
# that they should be removed.
#
# To reduce memory usage, arguments and locals can be accessed and
# allocated directly on the stack as well as in the normal locals tuple.
# These are the rules for when they use the stack:
# 
# 1) Arguments by default are accessed directly on the stack, as offsets
#    from the frame pointer.
# 1a) If an argument is stored to, it's converted to be a normal lvar.
# 1b) If an argument is accessed in a block (ie closed over), 
#     it's converted to be a normal lvar.
# 2) Locals are by default accessed, allocated, and stored on the stack.
# 2a) If a local is access or store in a block, it's converted to a normal
#     lvar.
# 
# Generalized:
# 3) Any lvar accessed in a block is converted to a normal lvar.
# 4) An argument lvar that is stored to is converted.

module Compiler1
class RsMethodBodyOnly < SimpleSexpProcessor
  
  def process_defs(x)
    y = x.dup
    x.clear
    if @auto_shift_type
      y[0] = process(y[0]) # the receiver of defs might be an lvar
      y.unshift :defs
    else
      y[1] = process(y[1])
    end
    
    return y
  end

  def process_defn(x)
    y = x.dup
    x.clear
    y.unshift :defn if @auto_shift_type
    return y
  end
  
  def process_class(x)
    y = x.dup
    x.clear
    y.unshift :class if @auto_shift_type
    return y
  end
  
  def process_module(x)
    y = x.dup
    x.clear
    y.unshift :module if @auto_shift_type
    return y
  end
  
  def process_case(x)
    x.shift unless @auto_shift_type
    cond = process(x.shift)
    whns = x.shift.map { |w| process(w) }
    els = process(x.shift)
    
    [:case, cond, whns, els]
  end
end

class RsLocalScoper < RsMethodBodyOnly
  
  class Phase1 < RsMethodBodyOnly
    def initialize(state)
      super()
      self.strict = false
      self.expected = Array
      
      @state = state
      @depth = 0
    end
    
    def process_iter(x)
      x.shift
      f = x.shift
      a = x.shift
      b = x.shift
      
      @depth += 1
      a = process(a)
      b = process(b)
      @depth -= 1
      
      [:iter, process(f), a, b]
    end
    
    def process_lvar(x)
      x.shift
      name = x.shift
      idx = x.shift
      @state.local(name).set_depth @depth
      [:lvar, name, idx]
    end
    
    def process_lasgn(x)
      x.shift
      name = x.shift
      idx = x.shift
      val = x.shift
      @state.local(name).set_depth @depth
      out = [:lasgn, name, idx]
      out << process(val) if val
      return out
    end
    
  end
  
    
  def initialize(state=RsLocalState.new)
    super()
    self.strict = false
    self.auto_shift_type = true
    self.expected = Array
    @state = state
    @bargs = []
    @masgn = []
    @in_block = false
    @call_scope = nil
  end
  
  def local(name)
    lv = @state.local(name)
    unless lv.scope
      if lv.min_depth == 0
        lv.scope = :normal
      else
        lv.scope = :block
      end
    end
    
    return lv
  end
  
  def inside_masgn
    @masgn.last.equal?(true)
  end
    
  def find_lvar(name, assign=false)
    return @state.find_local(name, true, assign)
=begin
    if idx = @state.locals.index(name)
      return idx
    end
    
    if inside_masgn and @bargs.last
      @bargs.last << name
    end
    
    cnt = @state.local(name)
    return cnt
=end
  end
  
  def setup_lvar(name, assign=false)
    lv = local(name)
    lv.assigned! if assign
    return lv
  end
  
  # Any local that still does not have a value can be allocated
  # on the stack.
  def formalize_lvars!
    @locals.each do |name, lv|
      unless lv.value?
        lv.set_value nil
      end
    end
  end
  
  def process_iter(x)
    f = x.shift
    a = x.shift
    b = x.shift
    
    a2 = nil
    b2 = nil

    ob = @in_block
    @in_block = true
    
    locals = @state.new_scope do
      @masgn.push true
      a2 = process(a)
      @masgn.pop
      b2 = process(b)
    end

    out = [:iter, process(f), a2, b2, locals]
    @in_block = ob
    return out
  end

  # Converts a for loop into an 'iter' that calls each
  # However, for loops have different scoping than blocks,
  # so we do not introduce a new scope
  def process_for(x)
    enum = x.shift
    lasgn = x.shift
    body = x.shift

    @masgn.push true
    @for_argument = true
    lasgn = process(lasgn)
    @for_argument = false
    @masgn.pop

    locals = Tuple.new(0)
    @state.new_scope do
      body = process(body)
    end

    body = [:block, body] if body.nil? || body.first != :block
    x = [:iter, [:call, process(enum), :each, [:array]], lasgn, body, locals]
  end

  def p_or_n(val)
    return nil if val.nil?
    return nil if val.equal?(true) # Crazy, but this is how anonymous splats look
    process(val)
  end

  def process_masgn(x)
    @masgn.push true
    m1 = p_or_n(x.shift)
    @masgn.pop
    m2 = p_or_n(x.shift)
    m3 = p_or_n(x.shift)
    return [:masgn, m1, m2, m3]
  end
  
=begin

  rubinius has no dvar/dasgn nodes, these were only used when
  we used sydparse.
  
  def process_dasgn_curr(x)
    name = x.shift
    val = x.shift
    
    # @state.disallow_stack_access(name) if @in_block
    lv = find_lvar(name, true)
    
    if inside_masgn
      [:lasgn, name, lv]
    else
      if not val
        raise "Incorrect usage of dasgn_curr '#{name}', needs a value. (#{val.inspect})"
      end
      
      [:lasgn, name, lv, process(val)]
    end
  end
  
  alias :process_dasgn :process_dasgn_curr
  
  def process_dvar(x)
    name = x.shift
    
    # @state.disallow_stack_access(name) if @in_block
    lv = find_lvar(name)
    [:lvar, name, lv]
  end
=end


  # 3) Any lvar accessed/stored in a block is converted to a normal lvar.

  def process_lvar(x)
    name = x.shift
    old = x.shift
    
    lv = local(name)
    
    lv.referenced!
    
    val = nil
    
    # If we're inside a block, we've only got a few choises..
    if @in_block
      lv.access_in_block!
      # Block scope'd vars have a access dependent location, so we
      # recalculate the position every access.
      
      val = find_lvar(name) if lv.scope == :block
    end
    
    if @call_scope == :recv
      lv.called_methods << @call_method
    elsif @call_scope == :args
      lv.passed_as_argument!
    end
    
    return [:lvar, name, lv, val]
  end
  
  # 3) Any lvar accessed/stored in a block is converted to a normal lvar.
  # 4) An argument lvar that is stored to is converted.
  
  def process_lasgn(x)
    name = x.shift
    old = x.shift
        
    lv = local(name)
    
    val = nil
    
    lv.assigned!
    
    # If we're inside a block, we've only got a few choises..
    if @in_block
      
      lv.access_in_block!
      
      # No scope means it's new, give it a scope.
      lv.scope = :block unless lv.scope
      
      if lv.scope == :block
        val = find_lvar(name)
        
        # This is unpleasant, but 'for' loops need to make use of 'create_block' 
        # without actually creating a nested scope for their arguments
        if @for_argument && Array === val
          val = [val[0]+1, val[1]]
        end
      end
    else
      lv.scope = :normal unless lv.scope
    end
    
    if inside_masgn
      x.clear
      out = [:lasgn, name, lv, val]
    else
      out = [:lasgn, name, lv, val, process(x.shift)]
    end
    
    return out
  end
  
  def process_vcall(x)
    name = x.shift

    if lv = @state.local(name, false) # don't allocate if it doesn't exist
      [:lvar, name, lv, nil]
    else
      [:call, [:self], name, [:array], {:function => true}]
    end
  end
  
  def process_call(x)
    recv = x.shift
    meth = x.shift
    args = x.shift
    options = x.shift
    
    cm = @call_method
    cs = @call_scope
    
    @call_method = meth
    @call_scope = :recv
    r2 = process(recv)    
    @call_scope = :args
    a2 = process(args)
    
    @call_scope = cs
    @call_method = cm
    
    [:call, r2, meth, a2, options]
  end
end
end

require 'sexp/simple_processor'
require 'translation/states'

# This converts a ruby 1.8.x sexp's idea of the scoping of 
# local variables into rubinius 1.0's idea.
# 
# The idea is the same as ruby 2.0 (or it was the last time I
# looked.)
# 1) Block arguments are scoped as local variables that are
#    only visible to the block that they are arguments for.
# 2) Any other local variable assignment is performed directly
#    into the lexical scope for the block.
# 2a) The upshot of this is the major change. In ruby 1.8.x
#     locals defined in a block we called 'dvars' and were
#     only visible in the block they were defined in.
#     In rubinius 1.0, those variables are visible outside
#     the block as well. There are NO dvars in rubinius.
# 3) Currently, block args are still allowed to shadow existing
#    locals so that they are visible outside the block.
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
# I think that for the meantime, I'm going to add another processor
# that will refactor all those into assigning the block argument
# into a local, then pull them back out and do the operation
# on the local like normal code.

class RsLocalScoper < SimpleSexpProcessor
  def initialize(state=RsLocalState.new)
    super()
    self.strict = false
    self.auto_shift_type = true
    self.expected = Array
    @state = state
    @bargs = []
    @masgn = []
  end
  
  def inside_masgn
    @masgn.last == true
  end
  
  def start_args
    @bargs << []
  end
  
  def clear_args
    cur = @bargs.last
        
    # Don't delete them, just set them to nil so that their space
    # is still calculated but they can't be seen.
    cur.each do |a|
      i = @state.locals.index(a)
      @state.locals[i] = [a, false]
    end
    @bargs.pop
  end
  
  def find_lvar(name)
    return @state.find_local(name)
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
  
  def process_iter(x)
    f = x.shift
    a = x.shift
    b = x.shift
    
    a2 = nil
    b2 = nil
    
    count = @state.new_scope do
      @masgn.push true
      # start_args
      a2 = process(a)
      @masgn.pop
      b2 = process(b)
      # clear_args
    end
        
    [:iter, process(f), a2, b2, count]
  end

  # Converts a for loop into an 'iter' that calls each
  # However, for loops have different scoping than blocks,
  # so we do not introduce a new scope
  def process_for(x)
    enum = x.shift
    lasgn = x.shift
    body = x.shift
    @masgn.push true
    lasgn = process(lasgn)
    @masgn.pop
    body = process(body)

    body = [:block, body] if body.nil? || body.first != :block
    # TODO - This use of @state may or may not be correct
    x = [[:call, process(enum), :each, [:array]], lasgn, body, @state.last_scope.size]
    x.unshift(:iter)
  end

  def p_or_n(val)
    return nil if val.nil?
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
  
  def process_dasgn_curr(x)
    name = x.shift
    val = x.shift
    if inside_masgn
      [:lasgn, name, find_lvar(name)]
    else
      if not val
        raise "Incorrect usage of dasgn_curr '#{name}', needs a value. (#{val.inspect})"
      end
      
      [:lasgn, name, find_lvar(name), process(val)]
    end
  end
  
  alias :process_dasgn :process_dasgn_curr
  
  def process_dvar(x)
    name = x.shift
    [:lvar, name, find_lvar(name)]
  end
  
  def process_lvar(x)
    name = x.shift
    old = x.shift
    [:lvar, name, find_lvar(name)]
  end
  
  def process_lasgn(x)
    name = x.shift
    old = x.shift
    if inside_masgn
      x.clear
      [:lasgn, name, find_lvar(name)]
    else
      [:lasgn, name, find_lvar(name), process(x.shift)]
    end
  end
  
  def process_vcall(x)
    name = x.shift
    idx = @state.find_local(name, false)
    if idx
      [:lvar, name, idx]
    else
      [:vcall, name]
    end
  end

  def process_defs(x)
    y = x.dup
    x.clear
    y[0] = process(y[0]) # the receiver of defs might be an lvar
    y.unshift :defs
    return y
  end

  def process_defn(x)
    y = x.dup
    x.clear
    y.unshift :defn
    return y
  end
  
  def process_class(x)
    y = x.dup
    x.clear
    y.unshift :class
    return y
  end
  
  def process_module(x)
    y = x.dup
    x.clear
    y.unshift :module
    return y
  end
  
  def process_case(x)
    cond = process(x.shift)
    whns = x.shift.map { |w| process(w) }
    els = process(x.shift)
    
    [:case, cond, whns, els]
  end
end

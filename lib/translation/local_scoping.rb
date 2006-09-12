require 'sexp/processor'
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

class RsLocalScoper < SexpProcessor
  def initialize(state=RsLocalState.new)
    super()
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
      @state.locals[i] = nil
    end
    @bargs.pop
  end
  
  def find_lvar(name)
    if idx = @state.locals.index(name)
      return idx
    end
    
    if inside_masgn
      @bargs.last << name
    end
    
    cnt = @state.local(name)
    return cnt
  end
  
  def process_iter(x)
    f = x.shift
    a = x.shift
    b = x.shift
    
    @masgn.push true
    start_args
    a2 = process(a)
    @masgn.pop
    b2 = process(b)
    clear_args
    
    [:iter, process(f), a2, b2]
  end

  def notprocess_masgn(x)
    out = [:masgn, process(x.shift), process(x.shift), process(x.shift)]
    @masgn.pop
    return out
  end
  
  def process_dasgn_curr(x)
    name = x.shift
    val = x.shift
    if inside_masgn
      [:lasgn, name, find_lvar(name)]
    else
      if not val
        raise "Incorrect usage of dasgn_curr #{name}, needs a value. (#{val.inspect})"
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
    idx = @state.locals.index(name)
    if idx
      [:lvar, name, idx]
    else
      [:vcall, name]
    end
  end
  
  def noprocess_defn(x)
    y = x.dup
    x.clear
    y.unshift :defn
    return y
  end
end
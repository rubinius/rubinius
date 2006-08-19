require 'sexp/processor'

class RsNormalizer < SexpProcessor
  def initialize
    super
    self.auto_shift_type = true
    self.expected = Array
    @lvars = Hash.new
    @lvar_count = 1
  end
  
  def process_iter(x)
    meth = process x.shift
    args = x.shift
    args = process args if args
    body = process x.shift
    if body[0] != :block
      body = [:block, body]
    end
    [:iter, meth, args, body]
  end
  
  def process_call(x)
    if x.size == 2
      return [:call, x.shift, x.shift, [:array]]
    else
      
      return [:call, x.shift, x.shift, x.shift]
    end
  end
  
  def process_fcall(x)
    out = process([:call, [:self], *x])
    x.clear
    return out
  end
  
  def process_vcall(x)
    return [:call, [:self], x.shift, [:array]]
  end
  
  def lvar_idx(name)
    unless idx = @lvars[name]
      idx = @lvar_count += 1
      @lvars[name] = idx
    end
    return idx
  end
  
  def process_lvar(x)
    name = x.shift
    idx = x.shift
    [:lvar, name, lvar_idx(name)]
  end
  
  def process_lasgn(x)
    name = x.shift
    idx = x.shift
    val = x.shift
    [:lasgn, name, lvar_idx(name), val]
  end
  
  # Turn all dvars into lvars. We're going to scope everything the same
  # as ruby 2.0, ie, no special dvar scoping.
  def process_dvar(x)
    name = x.shift
    return [:lvar, name, lvar_idx(name)]
  end
  
  alias :process_dvar_curr :process_dvar
  
  def process_dasgn(x)
    name = x.shift
    val = x.shift
    return [:lasgn, name, lvar_idx(name), val]
  end
    
  def process_zarray(x)
    x.clear
    return [:array]
  end
  
  def process_defn(x)
    name = x.shift
    body = x.shift
    
    block = body[1]
    args = block[1]
    
    if args.first != :args
      raise "Unknown defn layout."
    end
    
    if args.size == 1
      args += [[], [], nil, nil]
    end

    start = 2
    if block[2].first == :block_arg
      start = 3
      args << block[2]
    end
    
    block.replace block[start..-1].unshift(:block)
    [:defn, name, args, body]
  end
  
  def process_if(x)
    cond = x.shift
    thn = x.shift
    els = x.shift
    
    if thn and thn[0] != :block
      thn = [:block, thn]
    end
    
    if els and els[0] != :block
      els = [:block, els]
    end
    
    [:if, cond, thn, els]
  end
  
  def process_while(x, kind=:while)
    cond = x.shift
    body = x.shift
    
    if body and body[0] != :block
      body = [:block, body]
    end
    
    [kind, cond, body]
  end
  
  def process_until(x)
    process_while x, :until
  end
  
  def process_when(x)
    cond = x.shift
    body = x.shift
    
    if body[0] != :block
      body = [:block, body]
    end
    
    [:when, cond, body]
  end
  
end
require 'compiler1/sexp/simple_processor'
require 'compiler1/translation/states'

module Compiler1
class RsNormalizer < SimpleSexpProcessor
    
  def initialize(state=nil, full=false, lines=false)
    super()
    self.auto_shift_type = true
    self.expected = Array
    self.strict = false
    @state = state || RsLocalState.new
    @full = full
    @lines = lines
    @file = nil
    @line = nil
  end
  
  def location
    return nil unless @file
    "#{@file}:#{@line}"
  end
  
  def process(x)
    if @lines
      line = x.pop
      out = super(x)
      out.unshift line
      return out
    else
      super(x)
    end
  end
    
  def process_defs(x)
    recv = x.shift
    out = process_defn(x)
    out.shift
    out.unshift recv
    out.unshift :defs
    return out
  end
  
  def process_block(x)
    # This madness is that all the dvars allocated in a block are
    # indicate as a 'linked-list' of dasgn_curr nodes at the beginning
    # of the block.
    fn = x.first
    if fn and (fn.first == :dasgn_curr) and (!fn[2] or fn[2].first == :dasgn_curr)
      x.shift
    end
    out = [:block]
    while e = x.shift
      out << process(e)
    end
    out
  end

  def process_defn(x)
    name = x.shift
    body = x.shift

    # Detect that we're trying to normalize an already
    # normalized method...
    if real_body = x.shift
      return [:defn, name, body, real_body]
    end

    block = body[1]
    if block && block[0] == :args 
      # Insane defs like this:
      # def some_local_var.foo(x = ($foo_self = self; nil)); end
      body[1] = [:block, block]
      process_defn([name, body])
    elsif block && block[1][0] != :args
      raise "Unknown defn layout."
    end
    args = block[1] || []

    if args.size == 1
      args += [[], [], nil, nil]
    end

    #args[1] are required args
    #args[2] are optional args
    #args[3] is a splat, or nil
    #args[4] is a :block node that initializes the optional args
    # The :block is processed because it may contain vcall and fcall nodes
    args[4] = process(args[4]) if args[4]

    start = 2

    # block[2] could be nil if the method has no body
    if block[2] && block[2][0] == :block_arg
      start = 3
      args << block[2]
    end

    normalized_block = (block[start..-1] || []).unshift(:block)
    block.replace(normalized_block)
    if @full
      cur = @state
      @state = RsLocalState.new
      body = process(body)
      @state = cur
    end
    [:defn, name, args, body]
  end
  
  def process_iter(x)
    m = x.shift
    args = x.shift
    body = x.shift
    if body.nil?
      body = [:block]
    elsif body[0] != :block
      body = [:block, body]
    end
    
    if m == [:fcall, :loop]
      x.shift
      return [:loop, process(body)]
    end
    
    
    meth = process m
    oargs = process args if args
        
    # Detect the dasgn_curr declaration list as the first element
    # of the block.
    dasgn = body[1]
    if dasgn and dasgn.first == :dasgn_curr and 
            (dasgn[2].nil? or dasgn[2].first == :dasgn_curr)
        body[1] = nil
        body = body.compact
    end
    [:iter, meth, oargs, process(body)]
  end
  
  def process_vcall(x)
    #flag: [:binding]
    if x == [:binding]
      @state.uses_eval = true      
    end
    [:vcall] + x
  end

  # For some reason this can be asked to handle :newline nodes. 
  # TODO - Sanity check
  #[:call, [:const, :Hash], :[], [:newline, 1, "(eval)", [:splat, [:lvar, :x, 0]]]]
  def process_call(x)
    #skip: [[:lvar, :x, 0], :instance_eval]
    #skip: [[:self], :instance_eval]
    #skip: [[:lvar, :x, 0], :class_eval]
    #flag: [[:self], :instance_eval, [:array, [:str, "z"]]] 
    #flag: [[:lvar, :z, 0], :instance_eval, [:array, [:str, "z"]]]
    #flag: [[:self], :eval, [:array, [:str, "z"]]]
    #flag: [[:self], :binding] 
    msg = x[1]
    if msg == :eval or msg == :binding
      @state.uses_eval = true      
    elsif msg == :instance_eval or msg == :class_eval or msg == :module_eval
      if x[2] and x[2] != [:array] # no args indicates block form
        @state.uses_eval = true
      end
    end
    
    if x.size == 2
      recv = x.shift
      meth = x.shift
      if meth == :static and recv.kind_of?(Array) and recv.first == :str
        return [:static_str, recv.last]
      end
      return [:call, process(recv), meth, [:array], {}]
    else
      recv = x.shift
      meth = x.shift
      args = x.shift
      opts = x.shift
      opts = {} unless opts
      
      if args.first == :newline
        STDERR.puts "Unhandled newline node: #{args.inspect}" unless args.size == 4
        args = args[3]
      end

      if args.first == :argscat
        out = process(args)
      elsif args.first == :splat
        nw = [:argscat, [:array], args.last]
        out = process(nw)
      else
        args.shift
        out = [:array]
        args.each do |a|
          out << process(a)
        end
      end
      return [:call, process(recv), meth, out, opts]
    end
  end
    
  def process_fcall(x)
    sx = [:call, [:self], x.shift]
    args = x.shift
    sx << args if args
    out = process(sx)
    out.last[:function] = true
    x.clear
    return out
  end
  
  def process_zarray(x)
    x.clear
    return [:array]
  end
  
  def process_class(x)
    name = x.shift    
    sup = process(x.shift)
    body = x.shift
    if @full
      cur = @state
      @state = RsLocalState.new
      body = process(body)
      @state = cur
    end
    [:class, name, sup, body]    
  end
  
  def process_module(x)
    name = x.shift
    body = x.shift
    if @full
      cur = @state
      @state = RsLocalState.new
      body = process(body)
      @state = cur
    end
    [:module, name, body]    
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
    
    [:if, process(cond), process(thn), process(els)]
  end
  
  def process_scope(y)
    x = y.dup
    y.clear
    
    if x.size == 1
      vars = []
    else
      vars = x.pop
    end
    
    body = x.shift

    if x.size > 0
      body = [:block, body, x]
    elsif body and body.first != :block
      body = [:block, body]
    end

    body = process body
    
    [:scope, body, vars]
  end
  
  def process_while(x, kind=:while)
    cond = x.shift
    body = x.shift
    
    if body and body[0] != :block
      body = [:block, body]
    end
    
    style = x.shift
    
    [kind, process(cond), process(body), style]
  end
  
  def process_until(x)
    process_while x, :until
  end
  
  def process_newline(x)
    line = @line = x.shift
    file = @file = x.shift
    body = process(x.shift)
    [:newline, line, file, body]
  end
  
  def process_case(x)
    cond = process(x.shift)
    whns = x.shift.map { |w| process(w) }
    els = process(x.shift)
    
    [:case, cond, whns, els]
  end
  
  def process_when(x)
    cond = x.shift
    body = x.shift
    
    if body.nil? || body[0] != :block
      body = [:block, body]
    end
    
    [:when, process(cond), process(body)]
  end

  def process_many_if(x)
    branches = x[0]
    else_body = x[1]
    nested_ifs(branches, else_body)
  end
  
  def nested_ifs(branches, else_body)
    if branches.empty?
      process(else_body)
    else
      branch = branches.first
      conditions = branch[0]
      body = branch[1]
      [:if, disjoin_conditions(conditions), process(body), nested_ifs(branches[1..-1], else_body)]
    end
  end
  
  def disjoin_conditions(conditions)
    elements = conditions[1..-1].map {|elt| process(elt)}
    if elements.size > 1
      [:or] + elements
    else
      elements.first
    end
  end
end
end

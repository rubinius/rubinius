require 'sexp/processor'
require 'translation/type'

class LocalVar
  attr_accessor :name, :index, :type
end

class RsTyper < SexpProcessor
  
  def initialize(functions)
    super()
    @info = functions
    self.require_empty = false
    self.default_rewriter = :to_ts
    # self.require_expected = false
    self.expected = TypedSexp
    self.strict = true
    @local_vars = Hash.new
    @ivars = Hash.new
    @consts = @info.consts
    @self_type = []
    @current_klass = []
    @defns = []
    @current_iter = []
    @current_func = []
    @hints = Hash.new
  end
  
  attr_accessor :local_vars, :ivars, :consts, :self_type, :hints
  
  def normalize(sexp)
    state=RsLocalState.new
    comp = CompositeSexpProcessor.new
    comp << RsNormalizer.new(state, true)
    comp << RsLocalScoper.new(state)
    return comp.process(sexp)
  end
  
  def process_normalized(sexp)
    process normalize(sexp)
  end
  
  def process_function(klass, func)
    @local_vars = func.locals
    @ivars = klass.ivars
    @self_type = [klass]
    
    out = process func.body.first
    func.body = out
  end
  
  def add_local_var(name, idx, type)
    lv = LocalVar.new
    lv.name = name
    lv.index = idx
    lv.type = type
    @local_vars[name] = lv
  end
      
  def to_ts(x)
    return x if TypedSexp === x
    TypedSexp.new(*x)
  end
  
  def process_comment(x)
    str = x[1].strip
    if m = /T:\s*([\w\s]*) => (\w*)/.match(str)
      ret = $2
      args = $1.split(/\s*,\s*/).map { |a| Type.method_missing(a.strip) }
      hint = [::Type.method_missing(ret), args]
      return t(:hint, :type, hint)
    elsif m = /hint:\s*([\w\s]*)/.match(str)
      tags = $1.split(/\s*,\s*/)
      return t(:hint, :tag, tags)
    end
    return t()
  end
  
  def process_true(x)
    x.set_type Type.bool
  end
  
  def process!(exp, idx)
    exp[idx] = process(exp[idx])
  end
  
  alias :process_false :process_true
  
  def process_array(x)
    1.upto(x.size - 1) do |i|
      process! x, i
    end
    
    x.set_type Type.Array
  end
  
  def process_str(x)
    x.set_type Type.String
  end
  
  def process_nil(x)
    x.set_type Type.unknown
  end
  
  def process_self(x)
    cls = @self_type.last
    if cls.kind_of? Type
      type = cls
    else
      type = cls ? cls.type : Type.unknown
    end
    x.set_type(type)
  end
  
  def process_call(x)
    process! x, 1
    process! x, 3
    
    if x[1].kind_of? Array and x[1].first == :const and x[2] == :new
      name = x[1].last
      if true # @consts[name] == Type.Class
        if x[1].type.unknown?
          klass = add_meta(name)
          x[1].set_type klass.type
        end
        return x.set_type(Type.method_missing(name))
      end
    end

    func = @info.find(x[1].type, x[2]) rescue nil
    unless func
      # puts "Adding #{x[2].inspect} to #{x[1].type.inspect}"
      func = Function.new(x[2], nil, nil, Type.unknown)
      @info.add_function x[1].type, func
    end
    
    @called_func = func
    
    args = x[3][1..-1]
    typed = args.map { |i| i.type }
    begin
      func.check_args typed
    rescue Object => e
      raise "Unable to setup args for calling #{x[1].type}.#{x[2]}. #{e.message}"
    end
    x.set_type func.type
  end
  
  def process_attrasgn(x)
    process_call(x)
  end
  
  def process_super(x)
    name = @defns.last
    this_type = @self_type.last.type
    kls = @info.types[this_type]
    type = kls.super_type
    func = @info.find(type, name) rescue nil
    unless func
      @info.verify_type(type)
      # puts "Adding #{name} to #{type.inspect}"
      # puts "Adding #{x[2].inspect} to #{x[1].type.inspect}"
      func = Function.new(name, nil, nil, Type.unknown)
      @info.add_function type, func
    end
    if x.size == 1
      func.check_args []
    else
      process! x, 1
      func.check_args x[1][1..-1].map { |i| i.type }
    end
    x.set_type func.type
  end
  
  def process_scope(x)
    process! x, 1
    x.unify_with 1
  end
  
  def process_hint(x)
    puts "UNUSED HINT: #{x.inspect}"
    t()
  end
  
  def process_block(x)
    if x.size == 1
      return x.set_type(Type.void)
    end
    
    remove = []
    
    1.upto(x.size - 1) do |i|
      process! x, i
      if x[i].first == :hint
        @hint = x[i]
        # remove << i
        # puts "found hint #{x[i].inspect}."
      else
        @hint = nil
      end
    end

    unless remove.empty?
      remove.each { |idx| x.delete_at(idx) }
    end
    
    x.set_type x.last.type
  end
  
  def process_iter(x)    
    @current_iter << x
    process! x, 1
    
    args = x[2]
    if args.nil?
      args = []
    elsif args.first == :masgn
      args.shift
    else
      args = [args]
    end
    
    called = @called_func
    
    test = called.yield_args
    if test
      if test.size != args.size
        raise "Mis-match arg count on yield. #{args.size} for #{test.size}."
      end
      
      test = test.dup
    end
    
    args.each do |y|
      unless y.first == :lasgn
        raise "Unsupported iter argument. #{y.inspect}"
      end
      
      if test
        type = test.shift
      else
        type = Type.unknown
      end
      
      add_local_var y[1], y[2], type
    end
    
    out = process! x, 3
    
    if t = called.yield_type
      t.become(out.type)
    else
      called.yield_type = out.type
    end
    
    @current_iter.pop
    x.set_type x[1].type
  end
  
  def process_lasgn(x)
    process! x, 3
    x.unify_with 3
    if lt = @local_vars[x[1]]
      if x.type.unknown?
        x.type.become(lt.type)
      elsif lt.type.unknown?
        lt.type.become(x.type)
      elsif lt.type != x.type
        raise "Local variable type mismatch. '#{lt.type}' != '#{x.type}'"
      end
    else
      add_local_var x[1], x[2], x.type
    end
    return x
  end
  
  def process_lvar(x)
    lt = @local_vars[x[1]]
    raise "Unknown local '#{x[1]}'" unless lt
    x.set_type lt.type
  end
  
  def process_lit(x)
    x.set_type Type.new(x.last.class.name.to_sym)
  end
  
  def process_cdecl(x)
    process! x, 2
    x.unify_with 2
    @consts[x[1]] = x.type
    return x
  end
  
  def process_const(x)
    name = x[1]
    unless t = @consts[name]
      t = Type.unknown
      @consts[name] = t
    end
    x.set_type t
  end
  
  def process_colon2(x)
    if x.size == 2
      x.set_type @consts[x.last]
    else
      lhs = []
      path = x
      while path.size == 3
        lhs << path.last
        path = path[1]
      end
      lhs << path.last
      lhs << x.last
      const = lhs.join("::").to_sym
      x.set_type @consts[const]
    end
  end
  
  def process_iasgn(x)
    process! x, 2
    x.unify_with 2
    in_t = x.type
    
    name = x[1].to_s[1..-1]
    
    if t = @ivars[name]
      if t.unknown?
        t.become x.type
      elsif in_t.unknown?
        in_t.become(t)
      end
    else
      @ivars[name] = x.type
    end
    return x
  end
  
  def process_ivar(x)
    name = x[1].to_s[1..-1]

    unless t = @ivars[name]
      type = @self_type.last
      type = type.type if type
      unless t = @info.find_ivar(type, name)      
        t = Type.unknown
        @ivars[name] = t
      end
    end
    
    x.set_type t
  end
  
  def process_return(x)
    if x.size == 1
      x.set_type Type.nil
    else
      process! x, 1
      x.unify_with 1
    end
  end
  
  def process_if(x)
    process! x, 1
    process! x, 2 if x[2]
    process! x, 3 if x[3]
    if x[2] and x[3] and x[2].type != x[3].type
      if x[2].type.unknown?
        unless x[3].type.unknown?
          x[2].type.become(x[3].type)
          type = x[3].type
        end
      elsif x[3].type.unknown?
        x[3].type.become(x[2].type)
        type = x[2].type
      else
        type = [x[2].type, x[3].type]
      end
      x.set_type type
    elsif x[2]
      x.set_type x[2].type
    elsif x[3]
      x.set_type x[3].type
    else
      x.set_type Type.void
    end
  end
  
  def process_while(x)
    process! x, 1
    process! x, 2
    x.unify_with 2
  end
  
  alias :process_until :process_while
  
  def process_case(x)
    process! x, 1
    
    types = []
    whens = x[2]
    0.upto(whens.size - 1) do |i|
      process! whens, i
      types << whens[i].type
    end
    
    types.uniq!
    if types.size == 1
      x.set_type types.first
    else
      x.set_type types
    end
  end
  
  def process_when(x)
    process! x, 1
    process! x, 2
    x.unify_with 2
  end
  
  def process_class(x)
    process! x, 1
    if x[2]
      sup = x[2].last
      process! x, 2
    else
      sup = :Object
    end
    
    cur = @ivars
    klass = @info.add_klass x[1].last, sup
    @ivars = klass.ivars
    @self_type << klass
    process! x, 3
    @ivars = cur
    @self_type.pop
    x.set_type Type.void
  end
  
  def process_module(x)
    process! x, 1
    process! x, 2
    x.set_type Type.void
  end
  
  def process_args(x)
    x.set_type Type.void
  end
  
  def add_meta(name)
    meta_name = "#{name}Meta".to_sym
    klass = @info.add_klass meta_name, :Class
  end
  
  def process_defs(x)
    recv = x[1]
    name = x[2]
    args = x[3]
    
    if recv != [:self]
      raise "Unsupported defs reciever. #{recv.inspect}"
    end
    
    cls = @self_type.last.type
    
    raise "Not in a class!" unless cls
    
    klass = add_meta(cls.name)
    
    cur = @ivars
    
    @ivars = klass.ivars
    @self_type << klass
    
    mcls = klass.type
    
    @consts[cls.name] = mcls
    
    func = @info.find(mcls, name) rescue nil
    unless func
      func = Function.new name, nil, nil, Type.unknown
      @info.add_function mcls, func
    end
    
    out = do_def_body func, x, name, 3, 4
    
    @self_type.pop
    @ivars = cur
    return out
  end
  
  def process_defn(x)
    name = x[1]
    func = nil
    tags = nil
    if cls = @self_type.last
      if @hint and @hint[1] == :type
        ret = @hint.last.shift
        args = @hint.last.shift
        # puts "Applying hints to #{name} in #{cls.inspect}. Args: #{args.inspect} => #{ret.inspect}"
      elsif @hint and @hint[1] == :tag
        tags = @hint.last
        # puts "Hint tags for #{name} of #{cls.inspect}: #{tags.inspect}"
      else
        ret = nil
        args = nil
      end
      
      func = @info.find(cls.type, name) rescue nil
      
      unless func
        # puts "Adding #{name} to #{cls.inspect}, #{ret.inspect}, #{args.inspect}"
        func = Function.new name, nil, nil, ret || Type.unknown
        func.tags = tags
        @info.add_function cls.type, func
      else
        # puts "Rusing #{name} on #{cls.inspect}"
      end
      
      if args
        func.check_args args
      end
    end
    
    do_def_body func, x, name, 2, 3, args
  end
  
  def do_def_body(func, x, name, args=2, bidx=3, arg_types=nil)
    
    cur = @local_vars
    if func
      @local_vars = func.locals # Hash.new
    else
      @local_vars = Hash.new
    end
    
    @defns << name
    @current_func << func
    
    if x[args].size > 1
      # puts "#{name} => #{x[2][1].inspect}"
      idx = 2
      if arg_types and arg_types.size != x[args][1].size
        raise "Mis-match argument type count."
      end
      
      x[args][1].each do |y|
        if arg_types
          type = arg_types.shift
        else
          type = Type.unknown
        end
        add_local_var y, idx, type
        idx += 1
      end
    end
    body = process! x, bidx
    if func
      func.body = body[1][1..-1].set_type(body.type)
      if func.type
        if func.type.unknown?
          if body.type.unknown?
            func.type = body.type
          else
            func.type.become(body.type)
          end
        elsif body.type.unknown?
          body.type.become(func.type)
        elsif func.type != body.type
          raise "Type Mis-match #{func.type.inspect} != #{body.type.inspect}!"
        end
      else
        func.type = body.type
      end
    end
        
    @current_func.pop
    @defns.pop
    @local_vars = cur
    x.set_type Type.void
  end
  
  def process_yield(x)
    if x.size > 1
      process! x, 1
      if x[1].first == :array
        lst = x[1].dup
        lst.shift
      else
        lst = [x[1]]
      end
      types = lst.map { |t| t.type }
    else
      types = []
    end
    
    if func = @current_func.last
      func.yield_args = types
      type = func.yield_type = Type.unknown
    else
      type = Type.unknown
    end

    x.set_type type
  end
  
  def process_not(x)
    process! x, 1
    x.set_type Type.bool
  end
  
  def process_and(x)
    process! x, 1
    process! x, 2
    x.set_type Type.bool
  end
  
  def process_cvar(x)
    x.set_type Type.unknown
  end
  
  def process_cvasgn(x)
    x.set_type Type.unknown
  end
  
  def process_alias(x)
    x.set_type Type.void
  end
  
  def process_dstr(x)
    2.upto(x.size - 1) do |idx|
      process! x, idx
    end
    
    x.set_type Type.String
  end
  
  def process_evstr(x)
    process! x, 1
    x.set_type Type.String
  end
end
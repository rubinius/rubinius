require 'sexp/processor'

class Type
  def initialize(str)
    @name = str.to_sym
  end
  
  def to_s
    @name.to_s
  end
  
  attr_accessor :name
  
  def eql?(b)
    @name == b.name
  end
  
  alias :== :eql?
  
  def hash
    @name.hash
  end
  
  def unknown?
    @name == :unknown
  end
  
  TYPES = Hash.new
  
  class << self
    def method_missing(sym)
      TYPES[sym] ||= Type.new(sym)
    end
        
    def unknown
      Type.new(:unknown)
    end
  end  
end

# It's been deprecated long enough. I'm getting rid of it.
class Object
  undef :type
end

class TypedSexp < Sexp
  attr_accessor :type
  
  def set_type(t)
    @type = t
    return self
  end
  
  def unify_with(idx)
    set_type self[idx].type
    return self
  end
  
  def inspect
    str = map { |e| e.inspect }.join(", ")
    "t(#{str})"
  end
end

def t(*a)
  TypedSexp.new(*a)
end

class FunctionType
  def initialize(c_func, ret_type)
    @c_func = c_func
    @ret_type = ret_type
    @arg_types = nil
  end
  
  attr_accessor :c_func, :ret_type, :arg_types
  
  def check_args(args)
    unless @arg_types
      @arg_types = args
      return
    end
    
    if args != @arg_types
      raise "Mismatch argument types."
    end
  end
  
  def reset_args!
    @arg_types = nil
  end
  
  def copy_from(other)
    if @ret_type.name == :unknown
      @ret_type.name = other.ret_type.name
    end
    
    @c_func = other.c_func
  end
end

class FunctionTable
  def initialize
    @functions = Hash.new { |h,k| h[k] = {} }
  end
  
  def find(type, function)
    if ft = @functions[type][function]
      return ft
    end
    raise "Unable to find function '#{function}' for '#{type}'"
  end
  
  def add_function(type, function, ft)
    if cur = @functions[type][function]
      cur.copy_from ft
    else
      @functions[type][function] = ft
    end
  end
end

class LocalVar
  attr_accessor :name, :index, :type
end

class RsTyper < SexpProcessor
  
  def initialize(functions)
    super()
    @functions = functions
    self.require_empty = false
    self.default_rewriter = :to_ts
    self.require_expected = false
    self.strict = true
    @local_vars = Hash.new
    @ivars = Hash.new
  end
  
  attr_accessor :local_vars, :ivars
  
  def add_local_var(name, idx, type)
    lv = LocalVar.new
    lv.name = name
    lv.index = idx
    lv.type = type
    @local_vars[name] = lv
  end
      
  def to_ts(x)
    TypedSexp.new(*x)
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
    x.set_type Type.nil
  end
  
  def process_self(x)
    x.set_type Type.unknown
  end
  
  def process_call(x)
    process! x, 1
    process! x, 3
    func = @functions.find(x[1].type, x[2]) rescue nil
    unless func
      func = FunctionType.new(nil, Type.unknown)
      @functions.add_function x[1].type, x[2], func
    end
    func.check_args x[3][1..-1]
    x.set_type func.ret_type
  end
  
  def process_scope(x)
    process! x, 1
    x.unify_with 1
  end
  
  def process_block(x)
    1.upto(x.size - 1) do |i|
      process! x, i
    end
    
    x.set_type x.last.type
  end
  
  def process_iter(x)
    process! x, 1
    process! x, 3
    
    x.set_type x[1].type
  end
  
  def process_lasgn(x)
    process! x, 3
    x.unify_with 3
    if lt = @local_vars[x[1]]
      if lt.type != x.type
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
  
  def process_iasgn(x)
    process! x, 2
    x.unify_with 2
    @ivars[x[1]] = x.type
    return x
  end
  
  def process_ivar(x)
    x.set_type @ivars[x[1]]
  end
  
  def process_return(x)
    process! x, 1
    x.unify_with 1
  end
  
  def process_if(x)
    process! x, 1
    process! x, 2 if x[2]
    process! x, 3 if x[3]
    if x[2] and x[3] and x[2].type != x[3].type
      x.set_type [x[2].type, x[3].type]
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
  
end
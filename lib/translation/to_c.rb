require 'translation/normalize'
require 'translation/typer'
require 'sexp/processor'
require 'sexp/composite_processor'

class RsToC
  
  class << self
    alias :translator :new
  end
  
  def initialize(ft=nil, iv=nil)
    ft ||= FunctionTable.new
    iv ||= Hash.new
    
    @functions = ft
    @ivars = iv
    
    @translator = CompositeSexpProcessor.new
    @translator << RsNormalizer.new
    @translator << RsTyper.new(ft)
    @proc = RsToCProcessor.new(ft, iv)
    @translator << @proc
  end
  
  attr_accessor :functions, :ivars
  
  def process(x)
    out = @translator.process x
    @proc.scope_type ||= @proc.last_type
    return out
  end
  
  def ret_types
    @proc.ret_types
  end
  
  def scope_type
    @proc.scope_type
  end
  
  def return_type
    sc = scope_type
    rt = ret_types
    unless sc
      sc = rt.first
    end
    
    rt.each do |t|
      raise "Mismatch return types." unless sc == t
    end
        
    return sc
  end
end

class RsToCProcessor < SexpProcessor
  def self.translator(ft=nil, iv=nil)
    # return @translator if defined? @translator
    translator = CompositeSexpProcessor.new
    translator << RsNormalizer.new
    ft ||= FunctionTable.new
    iv ||= Hash.new
    class << translator
      attr_accessor :functions, :ivars
    end
    translator << RsTyper.new(ft)
    translator << RsToC.new(ft, iv)
    
    translator.ivars = iv
    translator.functions = ft
    
    return translator 
  end
  
  def initialize(ft, iv=Hash.new)
    super()
    @functions = ft
    self.auto_shift_type = true
    self.expected = String
    self.strict = true
    @unique_idx = 0
    @ivars = iv
    @ret_types = []
    @scope_type = nil
    @last_type = nil
  end
  
  attr_reader :functions, :ivars
  attr_accessor :scope_type, :ret_types, :last_type
  
  def process(x)
    @last_type = x.type if x
    super x
  end
  
  def unique
    "_rstoc_#{@unique_idx += 1}"
  end
  
  StaticTypes = {
    Type.bool => "BOOL",
    Type.int => "int",
  }
  
  def to_ctype(type)
    if ct = StaticTypes[type]
      return ct
    end
    
    raise "Unknown type '#{type}'"
  end
  
  def process_self(x)
    "self"
  end
  
  def process_true(x)
    "TRUE"
  end
  
  def process_false(x)
    "FALSE"
  end
  
  def process_nil(x)
    "NIL"
  end
  
  def process_call(x, b_name="NO_BLOCK")
    s_recv = x.shift
    recv = process s_recv
    meth = x.shift
    args = x.shift
    
    cargs = b_name.dup
        
    ft = @functions.find(s_recv.type, meth)
    
    if args.size > 1
      args.shift
      cargs << ", " + args.map { |a| process a }.join(", ")
    end
        
    "#{ft.c_func}(#{recv}, #{cargs})"
  end
  
  def process_scope(x)
    body = x.shift
    out = process body
    @scope_type ||= body.type
    return out
  end
  
  def process_block(x)
    output = []
    until x.empty?
      output << "#{process x.shift};"
    end
    return output.join("\n")
  end
    
  def process_iter(x)
    call = x.shift
    args = x.shift
    body_n = x.shift
    body = process body_n

    ret_type = to_ctype body_n.type
    b_name = unique
    output = "#{ret_type} #{b_name}() { return #{body} }"
    
    call.shift
    call_p = process_call call, b_name
    output << "\n" + call_p
    return output
  end
  
  def process_lit(x)
    obj = x.shift
    case obj
    when Fixnum
      obj.to_s
    else
      raise "Invalid literal object '#{obj}' #{obj.class}"
    end
  end
  
  def process_lasgn(x)
    name = x.shift
    idx = x.shift
    
    val = process x.shift
    
    "#{name} = #{val}"
  end
  
  def process_lvar(x)
    name = x.shift
    idx = x.shift
    
    name.to_s
  end
  
  def process_ivar(x)
    name = x.shift
    "self->#{name}"
  end
  
  def process_iasgn(x)
    name = x.shift
    val = x.shift
    type = val.type
    @ivars[name] ||= val.type
    
    s_val = process val
    
    "self->#{name} = #{s_val}"
  end
  
  def process_return(x)
    body = x.shift
    @ret_types << body.type
    
    s_body = process body
    "return #{s_body}"
  end
  
  def process_if(x)
    cond = process x.shift
    thn = process x.shift
    els = process x.shift
    
    "if(#{cond}) {\n  #{thn}\n} else {\n  #{els}\n}"
  end
  
  def process_while(x, kind="while")
    cond = process x.shift
    body = process x.shift
    
    "#{kind}(#{cond}) {\n  #{body}\n}"
  end
  
  def process_until(x)
    process_while x, "until"
  end
end
require 'translation/normalize'
require 'translation/typer'
require 'translation/states'
require 'sexp/processor'
require 'sexp/composite_processor'

class RsToC
  
  class << self
    alias :translator :new
  end
  
  def initialize(info, cg)
    @info = info
    @cg = cg
    
    @translator = CompositeSexpProcessor.new
    @translator << RsNormalizer.new
    @translator << (@typer = RsTyper.new(info))
    @proc = RsToCProcessor.new(info, cg)
    @translator << @proc
  end
  
  attr_accessor :info, :ivars, :typer
  
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

class TypeToC
  def initialize(ti)
    @ti = ti
  end
  
  def generate
    @ti.classes.each do |name, klass|
      klass.defined_methods.each do |name, func|
        pro = RsToCProcessor.new(@ti, @ti.cg)
        puts pro.process(func.body.shift)
      end
    end
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
  
  def initialize(ft, cg, prefix="")
    super()
    @info = ft
    @cg = cg
    self.auto_shift_type = true
    self.expected = String
    self.strict = true
    @unique_idx = 0
    @ret_types = []
    @scope_type = nil
    @last_type = nil
    @prefix = prefix
  end
  
  attr_reader :info, :ivars
  attr_accessor :scope_type, :ret_types, :last_type
  
  def process(x)
    @last_type = x.type if x
    super x
  end
  
  def unique
    "_rstoc_#{@unique_idx += 1}"
  end
  
  def to_ctype(type)
    return @info.to_c_instance(type)
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
  
  def process_const(x)
    x.shift
    "NULL"
  end
  
  Call = Struct.new(:func, :args, :cast, :recv, :recv_type)
  
  class Call
    def external?
      self.func.kind_of? TypeInfo::ExternalInfo
    end
  end
  
  def calculate_call(x)
    s_recv = x.shift
    meth = x.shift
    args = x.shift

    if meth == :new and s_recv.first == :const
      cf = @info.to_c_func(meth, s_recv.type)
      args.shift
      cargs = args.map { |i| process(i) }.join(", ")
      return "#{cf}(#{cargs})"
    end
    recv = process s_recv
    
    cast = false

    begin
      ft = @info.find(s_recv.type, meth)
    rescue RuntimeError => e
      raise "Unable to translate call of #{meth} on #{s_recv}. #{e.message}"
    end
    recv_type = s_recv.type
    
    if !ft or ft.type.unknown?
      klass = @info.classes[s_recv.type.name]
      while klass and (!ft or ft.type.unknown?)
        recv_type = klass.super_type
        ft = @info.find(recv_type, meth)
        if ft and !ft.type.unknown?
          cast = true
        else
          klass = Type::Klass.find(klass.superklass)
        end
      end
    end
    
    Call.new(ft, args, cast, recv, recv_type)
  end
  
  def process_call(x, block=nil)
    output_call calculate_call(x), block
  end
  
  def output_call(x, block=nil)
    return x if String === x
    
    if x.external?
      x.args.shift
      in_args = x.args.map { |a| process a }
      code = x.func.generate(@cg, x.recv, in_args, block)
    else
      # raise "Unknown function #{meth} for #{s_recv.type}."
    
      cargs = (block || "NO_BLOCK").dup
    
      if x.args.size > 1
        x.args.shift
        cargs << ", " + x.args.map { |a| process a }.join(", ")
      end
    
      cf = @info.to_c_func(x.func, x.recv_type)
      if !x.cast    
        code = "#{cf}(#{x.recv}, #{cargs})"
      else
        c_name = @info.to_c_instance(x.recv_type)
        code = "#{cf}((#{c_name})#{x.recv}, #{cargs})"
      end
    end
    
    return code
  end
  
  def process_scope(x)
    body = x.shift
    x.clear
    out = process body
    @scope_type ||= body.type
    return out
  end
  
  def process_block(x)
    output = []
    until x.empty?
      output << "#{@prefix}#{process x.shift};"
    end
    return output.join("\n")
  end
    
  def process_iter(x)
    csexp = x.shift
    csexp.shift
    
    call = calculate_call(csexp)
    
    args = x.shift
    body_n = x.shift
    body = process body_n
    
    if call.external?
      block = body
    else
      ret_type = to_ctype body_n.type
      b_name = unique
      @cg.add_block "#{ret_type} #{b_name}(struct _locals#{@cg.local_idx} *locals) { return #{body} }"
    
      block = "BLOCK(#{b_name})"
    end
    
    output_call(call, block)
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
    
    vn = x.shift
    @cg.add_local name, vn.type
    val = process vn
    
    "locals->#{name} = #{val}"
  end
  
  def process_lvar(x)
    name = x.shift
    idx = x.shift
    
    "locals->#{name}"
  end
  
  def process_ivar(x)
    name = x.shift.to_s[1..-1]
    "self->#{name}"
  end
  
  def process_iasgn(x)
    name = x.shift.to_s[1..-1]
    val = x.shift
    type = val.type
    
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
    node = x.shift
    type = node.type
    cond = process node
    body = process x.shift

    "#{kind}(#{cond}) {\n  #{body}\n}"
  end
  
  def process_until(x)
    process_while x, "until"
  end
  
  def process_and(x)
    left = process x.shift
    right = process x.shift
    "((#{left}) && (#{right}))"
  end
end
require 'cpu/runtime'
require 'sydparse'

class CPU::Primitives
  def initialize(cpu, con)
    @cpu = cpu
    @constructor = con
  end
  
  Primitives = [
    :noop,
    :add,
    :equal,
    :compare,
    :at,
    :put,
    :fields,
    :allocate,
    :create_block,
    :block_given,
    :block_call,
    :string_to_sexp,
    :load_file
  ]
  
  def self.name_to_index(name)
    Primitives.index(name)
  end
  
  def perform(prim)
    # 0 is sort of a noop primitive.
    
    meth = Primitives[prim]
    raise "Unknown primitive #{prim}" unless meth
    
    orig = @cpu.sp
    ret = send(meth)
    if !ret
      @cpu.sp = orig
    end
    
    return ret
  end
  
  def noop
    @cpu.stack_pop
    return true    
  end
  
  def add
    arg = @cpu.pop_object
    rec = @cpu.pop_object
    
    if !arg.fixnum? or !rec.fixnum?
      return false
    end
    
    obj = RObject.wrap(rec.to_int + arg.to_int)
    @cpu.push_object obj
    return true
  end
  
  def equal
    arg = @cpu.pop_object
    rec = @cpu.pop_object
    
    if !arg.fixnum? or !rec.fixnum?
      return false
    end
    
    if arg.to_int == rec.to_int
      ret = RObject.true
    else
      ret = RObject.false
    end
    
    @cpu.push_object ret
    return true
  end
  
  def compare
    arg = @cpu.pop_object
    rec = @cpu.pop_object
    
    if !arg.fixnum? or !rec.fixnum?
      return false
    end
    
    ret = rec.to_int <=> arg.to_int

    @cpu.push_object RObject.wrap(ret)
    return true
  end
  
  def at
    arg = @cpu.pop_object
    rec = @cpu.pop_object
    
    return false unless arg.fixnum?
    return false unless rec.reference?
    return false if rec.stores_bytes?
    
    int = arg.to_int
    return false unless int <= rec.fields
    
    obj = rec.at(arg.to_int)
    
    @cpu.push_object obj
    return true
  end
  
  def put
    idx = @cpu.pop_object
    arg = @cpu.pop_object
    rec = @cpu.pop_object
    
    return false unless idx.fixnum?
    return false unless rec.reference?
    return false if rec.stores_bytes?
    
    int = idx.to_int
    return false unless int <= rec.fields
    
    rec.put(int, arg)
    
    @cpu.push_object arg
    return true
  end
  
  def fields
    obj = @cpu.pop_object
    
    return false unless obj.reference?
    
    @cpu.push_object RObject.wrap(obj.fields)
    return true
  end
  
  def allocate
    cls = @cpu.pop_object
    
    return false unless cls.reference?
    
    cls.as :class
    
    count = cls.instance_fields
    
    return false unless count.fixnum?
    
    obj = @cpu.new_object cls, count.to_int
    
    @cpu.push_object obj
    return true
  end
  
  def create_block
    ctx = @cpu.pop_object
    lst = @cpu.pop_object
    
    return false unless ctx.reference?
    return false unless lst.fixnum?
    
    ctx.as :methctx
    
    blk = Rubinius::BlockContext.under_context ctx
    blk.last_op = lst
    
    @cpu.push_object blk
    return true
  end
  
  def block_given
    if @cpu.block.nil?
      @cpu.stack_push CPU::FALSE
    else
      @cpu.stack_push CPU::TRUE
    end
    return true
  end
  
  def block_call
    blk = @cpu.pop_object
    return false unless blk.reference?
    
    blk.as :blokctx
    
    blk.sender = @cpu.active_context
    blk.ip = blk.start_op
    @cpu.activate_context blk, blk.home
    return true
  end
  
  def string_to_sexp
    str = @cpu.pop_object
    return false unless str.reference?
    return false unless str.rclass == CPU::Global.string
    
    str.as :string
    syd = SydneyParser.load_string(str.as_string) rescue nil
    
    return false unless syd
    
    out = convert_sym_array(syd.sexp)
    @cpu.push_object out
    return true
  end
  
  def load_file
    str = @cpu.pop_object
    return false unless str.reference?
    return false unless str.rclass == CPU::Global.string
    
    str.as :string
    
    # TODO SydneyParser needs to have a way to return
    # and error without raising a CRuby exception so that
    # we can easily see that error here and then raise a
    # Rubinius exception.
    begin
      meth = @constructor.load_file str.as_string
    rescue LoadError => e
      exc = new_exception(CPU::Global.exc_loe, e.message)
      @cpu.exception = exc
      return false
    end
        
    @cpu.goto_method @cpu.main, meth, 0
    return true
  end
  
  private
  
  def convert_sym_array(ary)
    out = Rubinius::Array.new(ary.size)
    idx = 0
    ary.each do |i|
      case i
      when Array
        convert_sym_array(i)
      when Symbol
        str = Rubinius::String.new(i.to_s)
        out.set idx, str.to_sym
      end
      idx += 1
    end
    return out
  end
end
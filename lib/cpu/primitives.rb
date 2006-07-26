require 'cpu/runtime'

class CPU::Primitives
  def initialize(cpu)
    @cpu = cpu
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
    :create_block
  ]
  
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
end
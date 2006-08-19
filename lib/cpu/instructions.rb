require 'cpu/runtime'
require 'bytecode/encoder'
require 'cpu/primitives'

class CPU::Instructions
  def initialize(cpu)
    @cpu = cpu
    @primitives = CPU::Primitives.new(@cpu)
    @data = nil
    @current_address = 0
  end
  
  attr_accessor :data, :current_addres
  
  def run(bytes)
    @cpu.ip = 0
    @data = bytes
    total = @data.size
    
    while @cpu.ip < total
      dispatch
    end
  end
  
  def dispatch
    return false if @cpu.ip >= @data.size
    
    op = @data[@cpu.ip]
    meth = Bytecode::InstructionEncoder::OpCodes[op]
    raise "Unknown opcode '#{op}'" unless meth
  
    # puts "on #{op} / #{meth} (#{@cpu.ip})"
    @cpu.ip += 1
    send(meth)
    return true
  end
  
  IntSize = 4
  
  def next_int
    int = @data[@cpu.ip, IntSize].unpack("I").first
    @cpu.ip += IntSize
    return int
  end
  
  def noop
  end
  
  def push_nil
    @cpu.stack_push CPU::NIL
  end
  
  def push_true
    @cpu.stack_push CPU::TRUE
  end
  
  def push_false
    @cpu.stack_push CPU::FALSE
  end
  
  def push_context
    @cpu.push_object @cpu.active_context
  end
  
  def push_literal
    idx = next_int
    @cpu.push_object @cpu.literals.at(idx)
  end
  
  def push_self
    @cpu.push_object @cpu.self
  end
  
  def push_local
    idx = next_int
    val = @cpu.locals.at(idx)
    @cpu.push_object val
  end
  
  def push_exception
    @cpu.push_object @cpu.exception
  end
  
  def clear_exception
    @cpu.exception = RObject.nil
  end
  
  def push_block
    @cpu.push_object @cpu.block
  end
  
  def push_ivar
    sym = @cpu.literals.at next_int    
    obj = RObject.new @cpu.stack_pop
    obj.as :object
    val = obj.get_ivar sym
    @cpu.push_object val
  end
  
  def allocate
    idx = RObject.new(@cpu.stack_pop)
    obj = @cpu.new_blank idx.to_int
    @cpu.stack_push obj
  end
  
  def set_class
    obj = RObject.new(@cpu.stack_pop)
    klass = RObject.new(@cpu.stack_pop)
    
    obj.rclass = klass
    
    @cpu.stack_push obj.address
  end
  
  def store_field
    idx = RObject.new(@cpu.stack_pop)
    fel = RObject.new(@cpu.stack_pop)
    obj = RObject.new(@cpu.stack_pop)

    obj.put(idx.to_int, fel)
    
    @cpu.stack_push obj.address
  end
  
  def fetch_field
    idx = obj_top()
    obj = obj_top()
    
    out = obj.at(idx.to_int)
    
    @cpu.stack_push out.address
  end
  
  def send_primitive
    prim = next_int
    @primitives.perform(prim)
  end
  
  def goto
    ip = next_int
    @cpu.ip = ip
  end
  
  def goto_if_false
    ip = next_int
    top = @cpu.stack_pop
    if top == CPU::NIL or top == CPU::FALSE
      @cpu.ip = ip
    end
  end
  
  def goto_if_true
    ip = next_int
    top = @cpu.stack_pop
    if top != CPU::NIL and top != CPU::FALSE
      @cpu.ip = ip
    end
  end
  
  def goto_if_defined
    ip = next_int
    top = @cpu.stack_pop
    if top != CPU::UNDEF
      @cpu.ip = ip
    end
  end
  
  # Stack manipulation methods.
  
  def swap_stack
    o1 = @cpu.stack_pop
    o2 = @cpu.stack_pop
    @cpu.stack_push o1
    @cpu.stack_push o2
  end
  
  def dup_top
    @cpu.stack_push @cpu.stack_top
  end
  
  def pop
    @cpu.stack_pop
  end
  
  def set_local
    idx = next_int
    val = @cpu.stack_top
    @cpu.locals.put idx, RObject.new(val)
  end
  
  def make_array
    cnt = next_int
    ary = Rubinius::Array.new(cnt)
    start = cnt - 1
    start.downto(0) do |i|
      obj = @cpu.stack_pop
      ary.set i, RObject.new(obj)
    end
    
    @cpu.push_object ary
  end
  
  def push_array
    ary = @cpu.pop_object
    ary.as :array
    nd = ary.total.to_int - 1
    nd.downto(0) do |idx|
      @cpu.push_object ary.get(idx)
    end
  end
  
  def cast_array
    obj = @cpu.pop_object
    if obj.reference? and CPU::Global.tuple.address == obj.rclass.address
      obj = Rubinius::Array.from_tuple(obj)
    elsif !obj.reference? or CPU::Global.array.address != obj.rclass.address
      ary = Rubinius::Array.new(1)
      ary.set 0, obj
      obj = ary  
    end
    @cpu.push_object obj
  end
  
  def cast_tuple
    obj = @cpu.pop_object
    if obj.reference? and CPU::Global.array.address == obj.rclass.address
      obj.as :array
      tup = obj.tuple
    elsif !obj.reference? or CPU::Global.tuple.address != obj.rclass.address
      tup = Rubinius::Tuple.new(1)
      tup.put 0, obj
    else
      tup = obj
    end
    @cpu.push_object tup
  end
  
  def make_hash
    cnt = next_int
    ary = Rubinius::Hash.new
    cur = cnt - 1
    while cur >= 0
      key = @cpu.pop_object
      val = @cpu.pop_object
      ary.set key, val
      cur -= 2
    end
    
    @cpu.push_object ary
  end
  
  def set_ivar
    sym = @cpu.literals.at next_int    
    val = RObject.new @cpu.stack_pop
    obj = RObject.new @cpu.stack_pop
    obj.as :object
    obj.set_ivar sym, val
    @cpu.push_object val
  end
  
  def push_const
    sym = @cpu.literals.at next_int    
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    val = hsh.find(sym)
    @cpu.push_object val
  end
  
  def set_const
    sym = @cpu.literals.at next_int    
    val = RObject.new @cpu.stack_pop
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    hsh.set sym, val
    @cpu.push_object val
  end
  
  def find_const
    mod = RObject.new @cpu.stack_pop
    sym = @cpu.literals.at next_int
    
    mod.as :module
    hsh = mod.constants
    hsh.as :hash
    val = hsh.find sym
    @cpu.push_object val
  end
  
  def push_cpath_top
    @cpu.push_object CPU::Global.object
  end
  
  def set_encloser
    @cpu.enclosing_class = @cpu.pop_object
  end
  
  def open_class_under
    sup = @cpu.pop_object
    cont = @cpu.pop_object
    return open_class_at(cont, sup)
  end
  
  def open_class
    open_class_at @cpu.enclosing_class, @cpu.pop_object
  end
  
  def open_class_at(klass, sup)
    sym = @cpu.literals.at next_int    
    sup.as :class
    
    klass.as :module
    hsh = klass.constants
    hsh.as :hash
    
    exist = hsh.find sym
    if exist.nil?
      if sup.nil?
        sup = CPU::Global.object
      end
      
      cls = CPU::Global.class
      cls.as :class
      total = cls.instance_fields.to_int
      
      obj = @cpu.new_object cls, total
      obj.as :class
      obj.name = sym
      obj.superclass = sup
      obj.instance_fields = sup.instance_fields
      obj.metaclass.setup_fields
      
      hsh.set sym, obj

      @cpu.push_object obj
    else
      exist.as :class
      if !sup.nil? and exist.superclass != sup
        @cpu.stack_push CPU::NIL
      else
        @cpu.push_object exist
      end
    end
  end
  
  def open_module_under
    cont = @cpu.pop_object
    return open_module_at(cont)
  end
  
  def open_module
    open_module_at @cpu.enclosing_class
  end
  
  def open_module_at(klass)
    sym = @cpu.literals.at next_int    
    
    klass.as :module
    hsh = klass.constants
    hsh.as :hash
    exist = hsh.find sym
    
    if exist.nil?
      mod = CPU::Global.module
      mod.as :class
      
      total = mod.instance_fields.to_int
      
      obj = @cpu.new_object mod, total
      obj.as :module
      obj.name = sym
      obj.metaclass
      hsh.set sym, obj
      
      @cpu.push_object obj
      
    else
      @cpu.push_object exist
    end
  end
  
  def attach_method
    sym = @cpu.literals.at next_int
    target = @cpu.pop_object
    method = @cpu.pop_object
    meths = target.metaclass.methods
    meths.as :hash
    meths.set sym, method
    @cpu.push_object method
  end
  
  def add_method
    sym = @cpu.literals.at next_int    
    target = @cpu.pop_object
    method = @cpu.pop_object
    target.as :class
    meths = target.methods
    meths.as :hash
    meths.set sym, method
    @cpu.push_object method
  end
  
  def find_method(obj, name)
    klass = obj.rclass
    hsh = klass.methods
    hsh.as :hash
    
    meth = hsh.find(name)
    while meth.nil?
      klass = klass.superclass
      break if klass.nil?
      hsh = klass.methods
      hsh.as :hash
      meth = hsh.find(name)
    end
    
    return meth
  end
  
  def locate_method(recv, sym)
    @method_missing = Rubinius::String.new("method_missing").to_sym
    
    mo = find_method(recv, sym)
    if mo.nil?
      mo = find_method(recv, @method_missing)
      if mo.nil?
        raise "No method_missing found!"
      end
    end
    
    return mo
  end
  
  def create_context(recv, mo)
    mo.as :cmethod
    ctx = Rubinius::MethodContext.from_method(mo, 
              @cpu.active_context)
    ctx.receiver = recv
    return ctx
  end
  
  def activate_method
    recv = @cpu.pop_object
    meth = @cpu.pop_object
    ctx = create_context(recv, meth)
    ctx.argcount = RObject.wrap next_int()
    @cpu.activate_context ctx, ctx
  end
    
  def send_method
    if @cpu.literals.nil?
      raise "Literals are nil!"
    end
    sym = @cpu.literals.at next_int
    
    recv = @cpu.pop_object
    mo = locate_method recv, sym
    ctx = create_context recv, mo
    ctx.argcount = RObject.wrap(0)
    
    @cpu.activate_context ctx, ctx
  end
  
  def send_stack
    if @cpu.literals.nil?
      raise "Literals are nil!"
    end
    sym = @cpu.literals.at next_int
    args = next_int()
    
    recv = @cpu.pop_object
    
    mo = locate_method recv, sym
    ctx = create_context recv, mo
    ctx.argcount = RObject.wrap(args)
    
    @cpu.activate_context ctx, ctx
  end
  
  def send_stack_with_block
    if @cpu.literals.nil?
      raise "Literals are nil!"
    end
    sym = @cpu.literals.at next_int
    args = next_int()
    
    recv = @cpu.pop_object
    
    mo = locate_method recv, sym
    ctx = create_context recv, mo
    ctx.argcount = RObject.wrap(args)
    ctx.block = @cpu.pop_object
    
    @cpu.activate_context ctx, ctx
  end
  
  # Returns directly back to the sender, regardless of the type.
  # This is NOT a normal return which causes the home context
  # to return. This is used for things like break and next inside
  # a block. A block will always have one of these as it's last
  # bytecode.
  def soft_return
    @cpu.return_to_sender
  end
  
  # caller return is this strange instruction that actually returns
  # back to the current context sender's sender (ie, 2 levels up.)
  # It's used for a retry inside a block.
  def caller_return
    cur = @cpu.active_context
    cur.as :methctx
    @cpu.active_context = cur.sender
    @cpu.return_to_sender
  end
  
  def raise_exc
    exc = @cpu.pop_object
    raise_exception exc
  end
  
  def raise_exception(exc)
    @cpu.exception = exc
    ctx = @cpu.active_context
    
    until ctx.nil?
    
      # If the context isn't raisable, we just set the register
      # and return. The idea is this allows external code to examine
      # the exception and decide what it wants to do.
      if ctx.raiseable.false?
        return
      end
    
      table = @cpu.exceptions
      table.as :tuple
    
      raise "Invaild table" if table.nil?
    
      cur = @cpu.ip
      total = table.fields
      target = 0
      0.upto(total - 1) do |idx|
        ent = table.at(idx)
        if cur >= ent.at(0).to_int and cur <= ent.at(1).to_int
          target = ent.at(2).to_int
          @cpu.ip = target
          return
        end
      end
      
      ctx = ctx.sender
    end
    
    # TODO implement looking up the context chain for a handler.
    raise "Unable to find exception handler" if target == 0
    
  end
  
  def unshift_tuple
    tup = @cpu.pop_object
    if tup.fields == 0
      @cpu.push_object tup
      @cpu.stack_push CPU::NIL
      return
    end
    sz = tup.fields - 1
    obj = tup.at(0)
    t2 = Rubinius::Tuple.new(sz)
    tup.copy_fields_from t2, 1, sz
    @cpu.push_object t2
    @cpu.push_object obj
  end
  
  def make_rest
    req = next_int()
    total = @cpu.active_context.argcount.to_int - req
    ary = Rubinius::Array.new(total)
    0.upto(total - 1) do |idx|
      ary.set idx, @cpu.pop_object
    end
    @cpu.push_object ary
  end
  
  def check_argcount
    min = next_int()
    max = next_int()
    
    if @cpu.argcount < min
      msg = "wrong number of arguments (%d for %d)" % [@cpu.argcount, min]
      raise_exception new_exception(CPU::Global.exc_arg, msg)
    elsif max > 0 and @cpu.argcount > max
      msg = "wrong number of arguments (%d for %d)" % [@cpu.argcount, max]
      raise_exception new_exception(CPU::Global.exc_arg, msg)
    end
  end
  
  def passed_arg
    idx = next_int()
    if idx < @cpu.argcount
      @cpu.stack_push CPU::TRUE
    else
      @cpu.stack_push CPU::FALSE
    end
  end
  
  private
  
  def new_exception(klass, msg)
    klass.as :class
    obj = klass.new_instance
    str = Rubinius::String.new(msg)
    obj.put 0, str
    return obj
  end
  
  def obj_top
    RObject.new(@cpu.stack_pop)
  end
  
  def push_int
    obj = RObject.wrap(next_int)
    @cpu.stack_push obj.address
  end
  
end

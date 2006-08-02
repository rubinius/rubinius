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
    sym = RObject.symbol next_int
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
  
  def set_ivar
    sym = RObject.symbol next_int
    val = RObject.new @cpu.stack_pop
    obj = RObject.new @cpu.stack_pop
    obj.as :object
    obj.set_ivar sym, val
    @cpu.push_object val
  end
  
  def push_const
    sym = RObject.symbol next_int
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    val = hsh.find(sym)
    @cpu.push_object val
  end
  
  def set_const
    sym = RObject.symbol next_int
    val = RObject.new @cpu.stack_pop
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    hsh.set sym, val
    @cpu.push_object val
  end
  
  def find_const
    mod = RObject.new @cpu.stack_pop
    sym = RObject.symbol next_int
    mod.as :module
    hsh = mod.constants
    hsh.as :hash
    val = hsh.find sym
    @cpu.push_object val
  end
  
  def open_class
    sym = RObject.symbol next_int
    sup = @cpu.pop_object
    sup.as :class
    
    hsh = @cpu.enclosing_class.constants
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
  
  def attach_method
    sym = RObject.symbol next_int
    target = @cpu.pop_object
    method = @cpu.pop_object
    meths = target.metaclass.methods
    meths.as :hash
    meths.set sym, method
    @cpu.push_object method
  end
  
  def add_method
    sym = RObject.symbol next_int
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
  
  def send_stack
    @method_missing = Rubinius::String.new("method_missing").to_sym
    
    if @cpu.literals.nil?
      raise "Literals are nil!"
    end
    sym = @cpu.literals.at next_int
    
    recv = @cpu.pop_object
    mo = find_method(recv, sym)
    if mo.nil?
      mo = find_method(recv, @method_missing)
      if mo.nil?
        raise "No method_missing found!"
      end
    end
    
    mo.as :cmethod
    ctx = Rubinius::MethodContext.from_method(mo, 
              @cpu.active_context)
    ctx.receiver = recv
    
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
  
  private
  
  def obj_top
    RObject.new(@cpu.stack_pop)
  end
  
  def push_int
    obj = RObject.wrap(next_int)
    @cpu.stack_push obj.address
  end
  
end

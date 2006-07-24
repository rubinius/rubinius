require 'cpu/runtime'
require 'cpu/encoder'
require 'cpu/primitives'

class CPU::Instructions
  def initialize(cpu)
    @cpu = cpu
    @primitives = CPU::Primitives.new(@cpu)
  end
  
  def run(bytes)
    @cpu.ip = 0
    @data = bytes
    total = @data.size
    
    while @cpu.ip < total
      op = @data[@cpu.ip]
    
      meth = CPU::InstructionEncoder::OpCodes[op]
      raise "Unknown opcode '#{op}'" unless meth
    
      # puts "on #{op} / #{meth}"
    
      @cpu.ip += 1
      send(meth)
    end
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
  
  def push_int
    obj = RObject.wrap(next_int)
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
  
  def swap_stack
    o1 = @cpu.stack_pop
    o2 = @cpu.stack_pop
    @cpu.stack_push o1
    @cpu.stack_push o2
  end
  
  def set_local
    idx = next_int
    val = @cpu.stack_top
    @cpu.locals.put idx, RObject.new(val)
  end
  
  def push_local
    idx = next_int
    val = @cpu.locals.at(idx)
    @cpu.push_object val
  end
  
  def push_exception
    @cpu.push_object @cpu.exception
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
  
  def push_ivar
    sym = RObject.symbol next_int
    obj = RObject.new @cpu.stack_pop
    obj.as :object
    val = obj.get_ivar sym
    @cpu.push_object val
  end
  
  def set_ivar
    sym = RObject.symbol next_int
    val = RObject.new @cpu.stack_pop
    obj = RObject.new @cpu.stack_pop
    obj.as :object
    obj.set_ivar sym, val
    @cpu.push_object val
  end
  
  private
  
  def obj_top
    RObject.new(@cpu.stack_pop)
  end
end
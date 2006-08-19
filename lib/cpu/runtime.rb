require 'object_memory'
require 'types'
require 'ostruct'
require 'cpu/bootstrap'
require 'cpu/instructions'

class CPU
  
  InitialStackSize = 128
  FALSE = 0
  TRUE = 2
  NIL = 4
  UNDEF = 6
  
  Global = OpenStruct.new
  
  # Get rid of the normal #class method so we can use it to store the
  # our class object.
  class << Global
    undef :class
    undef :hash
  end
  
  def initialize
    Rubinius.cpu = self
    @memory = ObjectMemory.new
    @stack = Rubinius::Tuple.new_anonymous InitialStackSize
    @sp = -1
    @ip = 0
    @ms = 0
    @self = RObject.nil
    @exception = RObject.nil
    @enclosing_class = RObject.nil
    @new_class_of = RObject.nil
    
    # Some registers used to cache values taken from the 
    # active context.
    @locals = RObject.nil
    @literals = RObject.nil
    @block = RObject.nil
    @method = RObject.nil
    @argcount = 0
  end
  
  # This creates the top level MethodContext and also
  # fixes up any anonymous objects the cpu created 
  # pre-bootstrapping.
  def initialize_context
    @stack.rclass = Global.tuple
    @active_context = Rubinius::MethodContext.new(0)
    @home_context = @active_context
    @enclosing_class = Global.object
    @new_class_of = Global.class
    @exceptions = RObject.nil
    
    @primitives = CPU::Primitives.new(self)
    @data = nil
    @current_address = 0
  end
  
  def delete
    @memory.delete
  end
  
  attr_accessor :sp, :ip, :memory, :stack, :ms
  attr_accessor :active_context, :locals, :literals
  attr_accessor :self, :exception, :enclosing_class, :argcount
  attr_accessor :new_class_of, :block, :data, :exceptions
  attr_reader :instructions, :home_context
  
  def save_registers
    @active_context.sp = RObject.wrap(@sp)
    @active_context.ip = RObject.wrap(@ip)
    @active_context.ms = RObject.wrap(@ms)
  end
  
  def restore_context(x)
    restore_context_with_home x, x
  end
  
  def restore_context_with_home(ctx, home)
    ctx.as :methctx
    @sp = ctx.sp.to_int
    @ip = ctx.ip.to_int
    @ms = ctx.ms.to_int

    home.as :methctx
    ba = home.bytecodes
    if !ba.nil?
      ba.as :bytearray
      @data = ba.as_string
    end
    
    @active_context = ctx
    @home_context = home
    @locals = home.locals
    @block = ctx.block
    @method = home.method
    @method.as :cmethod
    @literals = home.literals
    @argcount = home.argcount.to_int
    
    if !@method.nil?
      @exceptions = @method.exceptions
    else
      @exceptions = RObject.nil
    end
  end
  
  def activate_context(ctx, home)
    save_registers
    restore_context_with_home(ctx, home)
  end
  
  def return_to_sender
    # puts "#{@active_context.address} is done."
    # We take the current top of the stack off and save
    # it. It's the return value.
    if @active_context.nil?
      raise "active_context is nil!"
    end
    
    @active_context.as :methctx
    sender = @active_context.sender
    # puts "switching to #{sender.address}"
    if sender.nil?
      @active_context = nil
    else
      # Support the ability for a method to have not done ANYTHING,
      # in which case we use nil as the return value.
      if stack_empty?
        top = CPU::NIL
      else
        top = stack_top
      end
      if Rubinius::BlockContext.block_context?(sender)
        sender.as :blokctx
        home = sender.home
        # puts "leaving a block context (#{home.address}, #{sender.address})!"
      else
        home = sender
      end
      restore_context_with_home(sender, home)
      # and now that @sp has been adjusted back to the destination
      # context's value, we push the return value in.
      stack_push top
    end
  end
  
  def run
    while @active_context
      # Dispatch returns false if there are no more bytecodes
      # in the current method. In this case, we go back to the
      # sender.
      if !dispatch
        return_to_sender
      end
    end
  end
  
  # A very raw method that injects bytecodes into the current context
  # and runs them until there are no more contexts to run.
  # Quite useful for testing.
  def execute_bytecodes(str)
    bc = Rubinius::ByteArray.from_string str
    @active_context.bytecodes = bc
    save_registers
    restore_context(@active_context)
    run
  end
  
  def stack_push(oop)
    if oop.respond_to?(:address)
      raise "Wrong method for #{oop}, use push_object"
    end
    @sp += 1
    Memory.store_long @stack.field_address(@sp), 0, oop
  end
  
  def push_object(obj)
    stack_push obj.address
  end
  
  def stack_pop
    raise "Stack is empty!" if stack_empty?
    out = Memory.fetch_long @stack.field_address(@sp), 0
    @sp -= 1
    return out
  end
  
  def pop_object
    RObject.new(stack_pop)
  end
  
  def stack_top
    raise "Stack is empty!" if stack_empty?
    Memory.fetch_long @stack.field_address(@sp), 0
  end
  
  def stack_empty?
    @sp == -1
  end
  
  def new_blank(fields)
    obj = @memory.new_object NIL, fields
    return obj.address
  end
  
  def new_object(cls, fields)
    return @memory.new_object(cls, fields)
  end
  
  def print_stack
    out = []
    0.upto(@sp) do |i|
      out << Memory.fetch_long(@stack.field_address(i), 0)
    end
    return "[#{out.join(", ")}]"
  end
  
  def local_get(num)
    obj = @locals.at num
    obj.address
    # idx = @ms - 1 + num
    # Memory.fetch_long(@stack.field_address(idx), 0)
  end
  
  def local_set(num, obj)
    obj = @locals.put num, RObject.new(obj)
    obj.address
    # idx = @ms - 1 + num
    # Memory.store_long @stack.field_address(idx), 0, obj    
  end
end
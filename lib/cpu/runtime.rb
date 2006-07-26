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
    @instructions = Instructions.new(self)
  end
  
  def delete
    @memory.delete
  end
  
  attr_accessor :sp, :ip, :memory, :stack, :ms
  attr_accessor :active_context, :locals, :literals
  attr_accessor :self, :exception, :enclosing_class
  attr_accessor :new_class_of, :block, :data
  attr_reader :instructions
  
  def save_registers
    @active_context.sp = RObject.wrap(@sp)
    @active_context.ip = RObject.wrap(@ip)
    @active_context.ms = RObject.wrap(@ms)
  end
  
  def restore_context(ctx)
    ctx.as :methctx
    @sp = ctx.sp.to_int
    @ip = ctx.ip.to_int
    @ms = ctx.ms.to_int
    
    ba = ctx.bytecodes
    if !ba.nil?
      ba.as :bytearray
      @instructions.data = ba.as_string
    end
    
    @active_context = ctx
    # @home_context = ctx.home
  end
  
  def activate_context(ctx)
    save_registers
    restore_context(ctx)
  end
  
  def run
    while @active_context
      # Dispatch returns false if there are no more bytecodes
      # in the current method. In this case, we go back to the
      # sender.
      if !@instructions.dispatch
        # puts "#{@active_context.address} is done."
        # We take the current top of the stack off and save
        # it. It's the return value.
        top = stack_top
        @active_context = @active_context.sender
        # puts "switching to #{@active_context.address}"
        if @active_context.nil?
          @active_context = nil
        else
          restore_context(@active_context)
          # and now that @sp has been adjusted back to the destination
          # context's value, we push the return value in.
          stack_push top
        end
      end
    end
  end
  
  # A very raw method that injects bytecodes into the current context
  # and runs them until there are no more contexts to run.
  # Quite useful for testing.
  def execute_bytecodes(str)
    bc = Rubinius::ByteArray.from_string str
    @active_context.bytecodes = bc
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
    idx = @ms - 1 + num
    Memory.fetch_long(@stack.field_address(idx), 0)
  end
  
  def local_set(num, obj)
    idx = @ms - 1 + num
    Memory.store_long @stack.field_address(idx), 0, obj    
  end
end
require 'cpu/runtime'
require 'bytecode/encoder'
require 'cpu/primitives'
require 'log'

class CPU
  attr_accessor :data, :current_address
  
  def run_bytes(bytes)
    @ip = 0
    @data = bytes
    total = @data.size
    
    while @ip < total
      dispatch
    end
  end
  
  def dispatch
    return unless @data
    return false if @ip >= @data.size
    
    op = @data[@ip]
    meth = Bytecode::InstructionEncoder::OpCodes[op]
    raise "Unknown opcode '#{op}'" unless meth
  
    Log.debug "on #{op} / #{meth} (#{@ip})"
    Log.debug "  =(stack)=> #{print_stack}"
    @ip += 1
    send(meth)
    return true
  end
  
  IntSize = 4
  
  def next_int
    int = @data[@ip, IntSize].unpack("i").first
    @ip += IntSize
    return int
  end
  
  def noop
  end
  
  def push_int
    int  = next_int()
    begin
      obj = RObject.wrap(int)
    rescue ArgumentError
      p int
      p @data[@ip-IntSize,IntSize]
      raise
    end
    stack_push obj.address
  end
  
  def push_nil
    stack_push CPU::NIL
  end
  
  def push_true
    stack_push CPU::TRUE
  end
  
  def push_false
    stack_push CPU::FALSE
  end
  
  def push_context
    @active_context.referenced!
    push_object @active_context
  end
  
  def push_literal
    idx = next_int
    push_object @literals.at(idx)
  end
  
  def push_self
    # Log.debug "Pushed self, which is #{@self.address}"
    push_object @self
  end
  
  def push_local
    idx = next_int
    val = @locals.at(idx)
    push_object val
    Log.debug "Pushed local at #{idx} (is #{val.address})"
  end
  
  def push_exception
    push_object @exception
  end
  
  def clear_exception
    @exception = RObject.nil
  end
  
  def push_block
    push_object @block
  end
  
  def push_ivar
    sym = @literals.at next_int    
    obj = @self
    obj.as :object
    val = obj.get_ivar sym
    push_object val
  end
  
  def allocate
    idx = RObject.new(stack_pop)
    obj = new_blank idx.to_int
    stack_push obj
  end
  
  def set_class
    obj = RObject.new(stack_pop)
    klass = RObject.new(stack_pop)
    
    obj.rclass = klass
    
    stack_push obj.address
  end
  
  def store_field
    idx = RObject.new(stack_pop)
    fel = RObject.new(stack_pop)
    obj = RObject.new(stack_pop)

    obj.put(idx.to_int, fel)
    
    stack_push obj.address
  end
  
  def fetch_field
    idx = pop_object()
    obj = pop_object()
    
    out = obj.at(idx.to_int)
    
    stack_push out.address
  end
  
  def send_primitive
    prim = next_int
    @primitives.perform(prim, nil)
  end
  
  def goto
    ip = next_int
    @ip = ip
  end
  
  def goto_if_false
    ip = next_int
    top = stack_pop
    if top == CPU::NIL or top == CPU::FALSE
      @ip = ip
    end
  end
  
  def goto_if_true
    ip = next_int
    top = stack_pop
    if top != CPU::NIL and top != CPU::FALSE
      @ip = ip
    end
  end
  
  def goto_if_defined
    ip = next_int
    top = stack_pop
    if top != CPU::UNDEF
      @ip = ip
    end
  end
  
  # Stack manipulation methods.
  
  def swap_stack
    o1 = stack_pop
    o2 = stack_pop
    stack_push o1
    stack_push o2
  end
  
  def dup_top
    stack_push stack_top
  end
  
  def pop
    stack_pop
  end
  
  def set_local
    idx = next_int
    val = stack_pop
    @locals.put idx, RObject.new(val)
    Log.debug "Set local #{idx} to #{val}."
  end
  
  def make_array
    cnt = next_int
    ary = Rubinius::Array.new(cnt)
    start = cnt - 1
    start.downto(0) do |i|
      obj = stack_pop
      ary.set i, RObject.new(obj)
    end
    
    push_object ary
  end
  
  def push_array
    ary = pop_object
    ary.as :array
    nd = ary.total.to_int - 1
    nd.downto(0) do |idx|
      push_object ary.get(idx)
    end
  end
  
  def cast_array
    obj = pop_object
    if obj.reference? and CPU::Global.tuple.address == obj.rclass.address
      obj = Rubinius::Array.from_tuple(obj)
    elsif !obj.reference? or CPU::Global.array.address != obj.rclass.address
      ary = Rubinius::Array.new(1)
      ary.set 0, obj
      obj = ary  
    end
    push_object obj
    return obj
  end
  
  def cast_array_for_args
    @args = next_int()
    ary = cast_array()
    @args += ary.as(:array).total.to_int
  end
  
  def cast_tuple
    obj = pop_object
    if obj.reference? and CPU::Global.array.address == obj.rclass.address
      obj.as :array
      tup = obj.tuple
    elsif !obj.reference? or CPU::Global.tuple.address != obj.rclass.address
      tup = Rubinius::Tuple.new(1)
      tup.put 0, obj
    else
      tup = obj
    end
    push_object tup
  end
  
  def make_hash
    cnt = next_int
    ary = Rubinius::Hash.new
    cur = cnt - 1
    while cur >= 0
      key = pop_object
      val = pop_object
      ary.set key, val
      cur -= 2
    end
    
    push_object ary
  end
  
  def set_ivar
    sym = @literals.at next_int    
    val = RObject.new stack_pop
    obj = @self
    obj.as :object
    obj.set_ivar sym, val
    push_object val
  end
  
  def push_const
    sym = @literals.at next_int    
    @enclosing_class.as :module
    hsh = @enclosing_class.constants
    hsh.as :hash
    sym.as :symbol
    val = hsh.find(sym)
    if val.nil?
      @paths.each do |cls|
        cls.as :module
        hsh = cls.constants
        hsh.as :hash
        val = hsh.find(sym)
        unless val.nil?
          push_object val
          return
        end
      end
    else
      # Log.debug "Pushed #{val.address} as const #{sym.as_string}"      
      push_object val
      return
    end
    
    raise "Unable to find const #{sym.as_string}"
  end
  
  def set_const
    sym = @literals.at next_int
    val = RObject.new stack_pop
    @enclosing_class.as :module
    hsh = @enclosing_class.constants
    hsh.as :hash
    sym.as :symbol
    hsh.set sym, val
    # puts "Set #{sym.as_string} to #{val} (#{val.to_int})"
    push_object val
  end
  
  def set_const_at
    sym = @literals.at next_int    
    val = RObject.new stack_pop
    under = pop_object()
    under.as :module
    hsh = under.constants
    hsh.as :hash
    hsh.set sym, val
    push_object val
  end
  
  def find_const
    mod = RObject.new stack_pop
    sym = @literals.at next_int
    
    mod.as :module
    hsh = mod.constants
    hsh.as :hash
    val = hsh.find sym
    push_object val
  end
  
  def push_cpath_top
    push_object CPU::Global.object
  end
  
  def set_encloser
    cls = pop_object
    if @paths.first == cls
      @paths.shift
    else
      @paths.unshift @enclosing_class
    end
    @enclosing_class = cls
  end
  
  def push_encloser
    push_object @enclosing_class
  end
  
  def open_class_under
    sup = pop_object
    cont = pop_object
    return open_class_at(cont, sup)
  end
  
  def open_class
    open_class_at @enclosing_class, pop_object
  end
  
  def open_class_at(klass, sup)
    sym = @literals.at next_int
    sup.as :class
    
    klass.as :module
    hsh = klass.constants
    hsh.as :hash
    
    sym.as :symbol
    exist = hsh.find sym
    if exist.nil?
      # Log.debug "Creating class #{sym.as_string}"
      if sup.nil?
        sup = CPU::Global.object
      end
      
      cls = CPU::Global.class
      cls.as :class
      total = cls.instance_fields.to_int
      
      obj = new_object cls, total
      obj.as :class
      obj.name = sym
      obj.superclass = sup
      obj.instance_fields = sup.instance_fields
      obj.metaclass.setup_fields
      obj.setup_fields
      
      hsh.set sym, obj

      push_object obj
      
      perform_hook sup, @inherited, obj
    else
      exist.as :class
      if !sup.nil? and exist.superclass != sup
        stack_push CPU::NIL
      else
        push_object exist
      end
      
      # Log.debug "Re-openning class #{sym.as_string}, #{exist.address}"
    end
  end
  
  def open_module_under
    cont = pop_object
    return open_module_at(cont)
  end
  
  def open_module
    open_module_at @enclosing_class
  end
  
  def open_module_at(klass)
    sym = @literals.at next_int    
    
    klass.as :module
    hsh = klass.constants
    hsh.as :hash
    exist = hsh.find sym
    
    if exist.nil?
      mod = CPU::Global.module
      mod.as :class
      
      total = mod.instance_fields.to_int
      
      obj = new_object mod, total
      obj.as :module
      obj.name = sym
      obj.metaclass
      obj.setup_fields
      hsh.set sym, obj
      
      push_object obj
      
    else
      push_object exist
    end
  end
  
  def attach_method
    sym = @literals.at next_int
    target = pop_object
    method = pop_object
    meths = target.metaclass.methods
    meths.as :hash
    meths.set sym, method
    push_object method
  end
  
  def add_method
    sym = @literals.at next_int
    target = pop_object
    method = pop_object
    target.as :class
    meths = target.methods
    meths.as :hash
    meths.set sym, method
    sym.as :symbol
    # puts "Added method #{sym.as_string} (#{sym.hash_int}) to #{target.address}."
    push_object method
    # perform_hook target, @method_added, sym 
  end
  
  def find_method(obj, name)
    klass = obj.logical_class
    hsh = klass.methods
    hsh.as :hash
    name.as :symbol
    klass.as :class
    
    Log.debug "Looking for #{name.as_string} in #{klass.as_string}, #{obj.as_string}"
    meth = hsh.find(name)
    while meth.nil?
      klass = klass.superclass
      klass.as :class
      break if klass.nil?
      Log.debug "Looking for #{name.as_string} (#{name.hash_int}) in #{klass.as_string}, #{obj.as_string}"
      hsh = klass.methods
      hsh.as :hash
      meth = hsh.find(name)
    end
    
    if klass.reference? and klass.name.symbol?
      Log.debug "Found #{name.as_string} in #{klass.name.as(:symbol).as_string}."
    end
    return meth
  end
  
  def locate_method(recv, sym)
    sym.as :symbol
    
    missing = false
    
    mo = find_method(recv, sym)
    if mo.nil?
      mo = find_method(recv, @method_missing)
      missing = true
      if mo.nil?
        raise "No method_missing found! #{sym.as_string} was lost!"
      end
    end
    
    mo.as :cmethod
    return [mo, missing]
  end
  
  def create_context(recv, mo, name)
    mo.as :cmethod
    if @active_context.nil?
      sender = RObject.nil
    else
      sender = @active_context
    end
    if @free_contexts.empty?
      ctx = Rubinius::MethodContext.from_method(mo, sender)
      # puts "Created new context #{ctx}..."
    else
      ctx = @free_contexts.pop
      # puts "Re-using context #{ctx} / #{@free_contexts.size}..."
      Rubinius::MethodContext.reuse(ctx, mo, sender)
    end
    ctx.receiver = recv
    ctx.name = name
    ctx.sp = RObject.wrap(@sp)
    return ctx
  end
  
  def activate_method
    recv = pop_object
    meth = pop_object
    meth.as :cmethod
    cnt = pop_object.to_int
    goto_method recv, meth, cnt, meth.name
  end
  
  def goto_method(recv, meth, count, name)
    return if try_primitive(meth, recv, count)
    ctx = create_context(recv, meth, name)
    ctx.argcount = RObject.wrap count
    activate_context ctx, ctx
  end
  
  def try_primitive(mo, recv, args)
    prim = mo.primitive.to_int
    req = mo.required.to_int
    ret = false
    if prim > -1
      if req < 0 or args == req
        push_object recv
        if @primitives.perform(mo.primitive.to_int, mo)
          Log.debug "Primitive response for #{prim} successful."
          return true
        else
          Log.debug "Pritive response for #{prim} FAILED."
        end
      elsif req >= 0 and CPU::Global.cmethod.address != mo.rclass.address
        msg = "wrong number of arguments (%d for %d) for primitive %d" % [args, req, prim]
        raise_exception new_exception(CPU::Global.exc_arg, msg)
        ret = true
      end
    end
    
    return ret
  end
  
  def perform_hook(recv, meth, arg)
    mo = find_method(recv, meth)
    return false if mo.nil?
    
    push_object arg
    ctx = create_context recv, mo, meth
    ctx.argcount = RObject.wrap(1)
    ctx.block = RObject.nil
    
    activate_context ctx, ctx
  end
  
  def unified_send(recv, idx, args, block)
    if @literals.nil?
      raise "Literals are nil!"
    end
    sym = @literals.at idx
    
    sym.as :symbol
    
    # Log.debug "#{@depth}: Sending #{sym.as_string} (#{idx}) to #{recv.address}."
    @depth += 1
    
    Log.debug "stack is #{print_stack} for #{sym.as_string} (#{recv.address}, #{args} args)"
    
    mo, missing = locate_method recv, sym
    return if try_primitive(mo, recv, args)
    if missing
      args += 1
      push_object sym
    end
    
    ctx = create_context recv, mo, sym
    # Log.debug "=> Created context #{ctx.address}"
    ctx.argcount = RObject.wrap(args)
    ctx.block = block
    
    activate_context ctx, ctx
  end
  
  def send_method
    unified_send pop_object, next_int, 0, RObject.nil
  end
      
  def send_stack
    sym = next_int()
    args = next_int()
    unified_send pop_object, sym, args, RObject.nil
  end
    
  def send_stack_with_block
    recv = pop_object
    blk = pop_object
    sym = next_int()
    args = next_int()
    unified_send recv, sym, args, blk
  end
  
  def send_with_arg_register
    recv = pop_object
    blk = pop_object
    sym = next_int()
    unified_send recv, sym, @args, blk
  end
    
  # Returns directly back to the sender, regardless of the type.
  # This is NOT a normal return which causes the home context
  # to return. This is used for things like break and next inside
  # a block. A block will always have one of these as it's last
  # bytecode.
  def soft_return
    return_to_sender(false)
  end
  
  # caller return is this strange instruction that actually returns
  # back to the current context sender's sender (ie, 2 levels up.)
  # It's used for a retry inside a block.
  def caller_return
    cur = @active_context
    cur.as :methctx
    @active_context = cur.sender
    return_to_sender
  end
  
  def raise_exc
    exc = pop_object
    raise_exception exc
  end
  
  def ret
    return_to_sender
  end
  
  def raise_exception(exc)
    @exception = exc
    # STDERR.puts "Exception raised: #{exc.at(0).as(:string).as_string}"
    ctx = @active_context
    
    # puts "Exception occurred, searching for handler..."
    until ctx.nil?
      
      # puts "Looking in #{ctx.address}, #{@exceptions.address}..."
      # If the context isn't raisable, we just set the register
      # and return. The idea is this allows external code to examine
      # the exception and decide what it wants to do.
      if ctx.raiseable.false?
        # puts "Hit non-raiseable ctx. stopping."
        return
      end
    
      table = @exceptions
      
      # If there is no table, then this method has no handlers.
      if table.nil?
        return_to_sender
        ctx = @active_context        
        next
      end
      
      table.as :tuple
    
      cur = @ip
      total = table.fields
      target = 0
      0.upto(total - 1) do |idx|
        ent = table.at(idx)
        if cur >= ent.at(0).to_int and cur <= ent.at(1).to_int
          target = ent.at(2).to_int
          @ip = target
          return
        end
      end
      
      return_to_sender(false)
      ctx = @active_context
    end
    
    # TODO implement looking up the context chain for a handler.
    raise "Unable to find exception handler" if target == 0
    
  end
  
  def unshift_tuple
    tup = pop_object
    if tup.fields == 0
      push_object tup
      stack_push CPU::NIL
      return
    end
    sz = tup.fields - 1
    obj = tup.at(0)
    t2 = Rubinius::Tuple.new(sz)
    tup.copy_fields_from t2, 1, sz
    push_object t2
    push_object obj
  end
  
  def make_rest
    req = next_int()
    total = @active_context.argcount.to_int - req
    ary = Rubinius::Array.new(total)
    # Log.debug "Making rest of #{total} items (req=#{req})"
    0.upto(total - 1) do |idx|
      ary.set idx, pop_object
    end
    push_object ary
  end
  
  def check_argcount
    min = next_int()
    max = next_int()
    
    if @argcount < min
      msg = "wrong number of arguments (%d for %d)" % [@argcount, min]
      raise_exception new_exception(CPU::Global.exc_arg, msg)
    elsif max > 0 and @argcount > max
      msg = "wrong number of arguments (%d for %d)" % [@argcount, max]
      raise_exception new_exception(CPU::Global.exc_arg, msg)
    end
  end
  
  def passed_arg
    idx = next_int()
    if idx < @argcount
      stack_push CPU::TRUE
    else
      stack_push CPU::FALSE
    end
  end
  
  def string_append
    recv = pop_object
    str =  pop_object
    recv.as :string
    recv.append str
    push_object recv
  end
  
  def string_dup
    recv = pop_object
    recv.as :string
    out = Rubinius::String.new(recv.as_string)
    push_object out
  end
  
  def set_args
    int = pop_object.to_int
    @args = int
  end
  
  def get_args
    push_object RObject.wrap(@args)
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
    RObject.new(stack_pop)
  end
    
end

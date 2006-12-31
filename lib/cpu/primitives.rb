require 'cpu/runtime'
require 'sydparse'
require 'platform'
require 'bytecode/primitive_names'

class CPU::Primitives
  def initialize(cpu, con)
    @cpu = cpu
    @constructor = con
  end
  
  Primitives = Bytecode::Compiler::Primitives
  FirstRuntimePrimitive = Bytecode::Compiler::FirstRuntimePrimitive
  RuntimePrimitives = Bytecode::Compiler::RuntimePrimitives
  
  def self.name_to_index(name)
    Primitives.index(name)
  end
  
  def perform(prim, mo)
    # 0 is sort of a noop primitive.
    if prim >= FirstRuntimePrimitive
      meth = RuntimePrimitives[prim - FirstRuntimePrimitive]
    else
      meth = Primitives[prim]
    end
    
    raise "Unknown primitive #{prim}" unless meth
    
    orig = @cpu.sp
    ret = send(meth, mo)
    if !ret
      @cpu.sp = orig
    end
    
    return ret
  end
  
  def noop(o)
    @cpu.stack_pop
    return true    
  end
  
  def add(o)
    rec = @cpu.pop_object
    arg = @cpu.pop_object
    
    if !arg.fixnum? or !rec.fixnum?
      return false
    end
    
    obj = RObject.wrap(rec.to_int + arg.to_int)
    @cpu.push_object obj
    return true
  end
  
  def sub(o)
    rec = @cpu.pop_object
    arg = @cpu.pop_object
    
    if !arg.fixnum? or !rec.fixnum?
      return false
    end
    
    obj = RObject.wrap(rec.to_int - arg.to_int)
    @cpu.push_object obj
    return true
  end
  
  def equal(o)
    rec = @cpu.pop_object
    arg = @cpu.pop_object
    
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
  
  def compare(o)
    rec = @cpu.pop_object
    arg = @cpu.pop_object
    
    if !arg.fixnum? or !rec.fixnum?
      return false
    end
    
    ret = rec.to_int <=> arg.to_int

    @cpu.push_object RObject.wrap(ret)
    return true
  end
  
  def at(o)
    rec = @cpu.pop_object
    arg = @cpu.pop_object
    
    return false unless arg.fixnum?
    return false unless rec.reference?
    return false if rec.stores_bytes?
    
    int = arg.to_int
    return false unless int <= rec.fields
    
    obj = rec.at(arg.to_int)
    
    # puts "Retrieved a #{obj.rclass.name.as(:symbol).as_string} from #{rec.rclass.name.as(:symbol).as_string} at #{arg.to_int}"
    
    @cpu.push_object obj
    return true
  end
  
  def put(o)
    rec = @cpu.pop_object
    idx = @cpu.pop_object
    arg = @cpu.pop_object    
    
    return false unless idx.fixnum?
    return false unless rec.reference?
    return false if rec.stores_bytes?
    
    int = idx.to_int
    return false unless int <= rec.fields
    
    rec.put(int, arg)
    
    @cpu.push_object arg
    return true
  end
  
  def fields(o)
    obj = @cpu.pop_object
    
    return false unless obj.reference?
    
    @cpu.push_object RObject.wrap(obj.fields)
    return true
  end
  
  def allocate(o)
    cls = @cpu.pop_object
    
    return false unless cls.reference?
    
    cls.as :class
    
    count = cls.instance_fields
    
    return false unless count.fixnum?
    
    obj = @cpu.new_object cls, count.to_int
    
    @cpu.push_object obj
    return true
  end
  
  def allocate_count(o)
    cls = @cpu.pop_object
    count = @cpu.pop_object
    
    return false unless cls.reference?
    
    cls.as :class
    
    return false unless count.fixnum?
    
    obj = @cpu.new_object cls, count.to_int
    
    @cpu.push_object obj
    return true
  end
  
  def allocate_bytes(o)
    cls = @cpu.pop_object
    bytes = @cpu.pop_object
    
    return false unless cls.reference?
    
    cls.as :class
    
    return false unless bytes.fixnum?
    
    obj = @cpu.new_object cls, bytes.to_int / 4
    obj.make_byte_storage
    
    @cpu.push_object obj
    return true
  end
  
  def create_block(o)
    ctx = @cpu.pop_object
    lst = @cpu.pop_object
    vlst = @cpu.pop_object
    return false unless ctx.reference?
    return false unless lst.fixnum?
    return false unless vlst.fixnum?
    
    ctx.as :methctx
    
    ctx.ip = RObject.wrap(@cpu.ip)
    
    blk = Rubinius::BlockEnvironment.under_context ctx, lst, vlst

    # puts "Block #{blk.address} created under #{ctx.address}"
    @cpu.push_object blk
    return true
  end
  
  def block_given(o)
    if @cpu.block.nil?
      @cpu.stack_push CPU::FALSE
    else
      @cpu.stack_push CPU::TRUE
    end
    return true
  end
  
  def block_call(o)
    env = @cpu.pop_object
    return false unless env.reference?
    
    env.as :blokenv
    
    blk = env.create_context
    blk.as :blokctx
    
    blk.sender = @cpu.active_context
    blk.sp = RObject.wrap(@cpu.sp)
    
    meth = RObject.new(blk.address)
    meth.as :methctx
    # puts "Calling block #{blk.address}, home of #{blk.home.address} / #{meth.sender.address}"
    @cpu.activate_context blk, blk.home
    return true
  end
  
  def string_to_sexp(o)
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
  
  def load_file(o)
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
        
    @cpu.goto_method @cpu.main, meth, 0, RObject.nil
    return true
  end
  
  def io_write(o)
    Log.debug "Writing data.."
    io = @cpu.pop_object
    str = @cpu.pop_object
    
    return false unless io.reference?
    return false unless io.rclass == CPU::Global.io
    
    io.as :io
        
    return false unless str.reference?
    return false unless str.rclass == CPU::Global.string
    
    str.as :string
    
    fd = io.descriptor
    rstr = str.as_string
    
    Log.debug "Writing #{rstr.inspect} to #{fd}."
    
    Platform.io_write fd.address, str.bytes.address, str.data.address
    # Platform.io_write fd.to_int, rstr.size, rstr
    return true
  end
  
  def io_read(o)
    io = @cpu.pop_object
    cnt = @cpu.pop_object
    
    return false unless io.reference?
    return false unless io.rclass == CPU::Global.io
    
    io.as :io
    
    return false unless cnt.fixnum?
    
    fd = io.descriptor
    num = cnt.to_int
    rstr = Rubinius::String.new_empty(num)
    total = Platform.io_read fd.address, cnt.address, rstr.data.address
        
    if total != num
      obj = Rubinius::String.new_empty(total)
      Memory.transfer_memory rstr.bytes_address, total, obj.bytes_address
      rstr = obj
    end
    @cpu.push_object rstr
    return true
  end
  
  def create_pipe(o)
    cls = @cpu.pop_object
    lhs = @cpu.pop_object
    rhs = @cpu.pop_object
    
    return false unless lhs.reference?
    return false unless lhs.rclass == CPU::Global.io
    
    return false unless rhs.reference?
    return false unless rhs.rclass == CPU::Global.io
    
    out = Platform.create_pipe lhs.address, rhs.address
    @cpu.push_object RObject.wrap(out)
    return true
  end
  
  def gettimeofday(o)
    obj = @cpu.pop_object
    
    return false unless obj.reference?
    return false unless obj.fields > 1
    
    out = Platform.fill_time obj.address
    @cpu.push_object RObject.wrap(out)
    return true
  end
  
  def strftime(o)
    obj = @cpu.pop_object
    format = @cpu.pop_object
    
    format.as :string
    
    rstr = Rubinius::String.new_empty(100)
    
    return false unless obj.reference?
    return false unless obj.fields > 1
    return false unless format.reference?
    
    out = Platform.print_time obj.address, format.data.address, rstr.data.address, rstr.bytes.address
    @cpu.push_object rstr
    return true
  end
  
  def get_time_info(o)
    @cpu.pop_object
    
    tup = Rubinius::Tuple.new(4)
    sz = Platform.time_data_size
    tup.put 0, RObject.wrap(sz)
    
    @cpu.push_object tup
  end
  
  def fixnum_to_s(o)
    obj = @cpu.pop_object
    return false unless obj.fixnum?
    base = @cpu.pop_object
    return false unless base.fixnum?
    
    int = obj.to_int
    str = Rubinius::String.new(int.to_s(base.to_int))
    @cpu.push_object str
    return true
  end
  
  def logical_class(o)
    obj = @cpu.pop_object
    cls = obj.real_class
    @cpu.push_object cls
    return true
  end
  
  def object_id(o)
    obj = @cpu.pop_object
    @cpu.push_object RObject.wrap(obj.address)
    return true
  end
  
  def hash_set(o)
    obj = @cpu.pop_object
    hsh = @cpu.pop_object
    
    return false unless obj.reference?
    # return false unless obj.rclass == CPU::Global.hash
    return false unless hsh.fixnum?
    
    key = @cpu.pop_object
    val = @cpu.pop_object
    obj.as :hash
    obj.add hsh.to_int, key, val
    @cpu.push_object val
    return true
  end
  
  def hash_get(o)
    obj = @cpu.pop_object
    hsh = @cpu.pop_object
    
    return false unless obj.reference?
    # I've commented this out so that hash_get works
    # on hash subclasses. I'll need to use the same
    # thing that Class#< uses.
    # return false unless obj.rclass == CPU::Global.hash
    return false unless hsh.fixnum?
    
    key = @cpu.pop_object
    obj.as :hash
    val = obj.get_unamigious(hsh.to_int)
    
    @cpu.push_object val
    return true
  end
  
  def hash_object(o)
    obj = @cpu.pop_object
    out = obj.hash_int
    @cpu.push_object RObject.wrap(out)
    return true
  end
  
  def symbol_index(o)
    sym = @cpu.pop_object
    return false unless sym.symbol?
    idx = RObject.wrap(sym.symbol_index)
    # puts "Symbol index is #{sym.symbol_index}, #{idx.address}"
    @cpu.push_object idx
    return true
  end
  
  def symbol_lookup(o)
    str = @cpu.pop_object
    str.as :string
    @cpu.push_object str.to_sym
    return true
  end
  
  def dup_into(o)
    obj = @cpu.pop_object
    into = @cpu.pop_object
    return false unless obj.reference?
    return false unless into.reference?
    
    into.copy_fields obj, into.fields
    @cpu.push_object obj
    return true
  end

  def fetch_bytes(o)
    ba = @cpu.pop_object
    start = @cpu.pop_object
    count = @cpu.pop_object
    
    return false unless start.fixnum?
    return false unless count.fixnum?
    return false unless ba.reference?
    
    count_i = count.to_int
    start_i = start.to_int
    
    return false if count_i < 0
    return false if start_i < 0
    
    ba.as :bytearray
    
    return false if ba.bytes <= (count_i - start_i)
        
    nw = Rubinius::ByteArray.new(count_i)
    
    ba.copy_bytes_into nw, count_i, start_i
    
    @cpu.push_object nw
    
    return true
  end
  
  def compare_bytes(o)
    me = @cpu.pop_object
    oth = @cpu.pop_object
    
    return false unless me.reference?
    return false unless oth.reference?
        
    me.as :bytearray
    oth.as :bytearray
    
    if me.bytes != oth.bytes
      @cpu.stack_push CPU::FALSE
    elsif me.bytes == 0
      @cpu.stack_push CPU::TRUE
    else
      out = Memory.compare_memory me.byte_start, oth.byte_start, me.bytes
      @cpu.push_object RObject.wrap(out)
    end
    
    return true
  end
  
  def get_ivar(mo)
    obj = @cpu.pop_object
    obj.as :object
    
    sym = mo.at(3)
    sym.as :symbol
    
    val = obj.get_ivar sym
    
    @cpu.push_object val
    return true
  end
  
  def set_ivar(mo)
    obj = @cpu.pop_object
    obj.as :object
    
    val = @cpu.pop_object
    
    sym = mo.at(3)
    sym.as :symbol
    
    obj.set_ivar sym, val
    @cpu.push_object val
    return true
  end
  
  def get_index(mo)
    obj = @cpu.pop_object
    obj.as :object
    
    idx = mo.at(3).to_int
    
    val = obj.at(idx)
    
    @cpu.push_object val
    return true
  end
  
  def set_index(mo)
    obj = @cpu.pop_object
    obj.as :object
    
    idx = mo.at(3).to_int
    
    val = @cpu.pop_object
    obj.put(idx, val)
    
    @cpu.push_object val
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

require 'rubygems'
require 'test/unit'
require 'cpu/instructions'
require 'cpu/runtime'
require 'bytecode/encoder'
require 'test/unit/show_code'

class TestCPUInstructions < Test::Unit::TestCase
  
  def setup
    @encoder = Bytecode::InstructionEncoder.new
    @cpu = CPU.new
    @inst = CPU::Instructions.new(@cpu)
    @cpu.bootstrap
  end
  
  def teardown
    @cpu.delete
  end
  
  def enc(*args)
    @encoder.encode *args
  end
  
  def exec(bc)
    @cpu.execute_bytecodes bc
  end
  
  def test_run
    params = enc(:push_int, 1) + enc(:push_int, 2) + enc(:push_int, 100)
    @inst.run params
    assert_equal 2, @cpu.sp
  end
    
  def test_noop
    @inst.run enc(:noop)
    assert_equal 1, @cpu.ip
  end
  
  def test_push_nil
    @inst.run enc(:push_nil)
    assert_equal 1, @cpu.ip
    assert_equal 0, @cpu.sp
    assert_equal CPU::NIL, @cpu.stack_top
  end
  
  def test_allocate
    @inst.run enc(:allocate, 10)
    assert !@cpu.stack_empty?
    obj = RObject.new(@cpu.stack_top)
    assert_equal 10, obj.fields
    assert obj.rclass.nil?
  end
  
  def test_set_class
    @inst.run enc(:allocate, 4)
    klass = @cpu.stack_top
    @inst.run enc(:allocate, 4)
    obj = @cpu.stack_top
    csp = @cpu.sp
    @inst.run enc(:set_class)
    
    robj = RObject.new(obj)
    assert_equal csp - 1, @cpu.sp
    assert_equal klass, robj.rclass.address
    assert_equal obj, @cpu.stack_top
  end
  
  def test_push_int
    @inst.run enc(:push_int, 88)
    robj = RObject.new @cpu.stack_top
    assert robj.fixnum?
    assert_equal 88, robj.to_int
  end
  
  def test_store_field
    @inst.run enc(:allocate, 4)
    obj = @cpu.stack_top
    @inst.run enc(:push_int, 88)
    @inst.run enc(:push_int, 1)
    @inst.run enc(:store_field)
    robj = RObject.new @cpu.stack_top
    assert_equal obj, robj.address
    
    int = robj.at(1)
    assert int.fixnum?
    assert_equal 88, int.to_int
  end
  
  def test_store_field_at
    @inst.run enc(:allocate, 4)
    @inst.run enc(:push_int, 89)
    @inst.run enc(:store_field_at, 1)
    
    robj = RObject.new @cpu.stack_top
    int = robj.at(1)
    assert_equal 89, int.to_int
  end
  
  def obj_top
    RObject.new @cpu.stack_top
  end
  
  def test_fetch_field
    fel = 2
    @inst.run enc(:allocate, 4)
    @inst.run enc(:push_int, 99)
    @inst.run enc(:push_int, fel)
    @inst.run enc(:store_field)
    @inst.run enc(:push_int, fel)
    @inst.run enc(:fetch_field)
    
    robj = obj_top
    
    assert_equal 99, robj.to_int
  end
  
  def test_send_primitive
    @inst.run enc(:allocate, 4)
    @inst.run enc(:send_primitive, 0)
    assert @cpu.stack_empty?
  end
  
  def test_push_context
    @inst.run enc(:push_context)
    assert_equal @cpu.active_context, obj_top
  end
  
  def test_push_literal
    obj = Rubinius::Tuple.new(1)
    lcls = Rubinius::Tuple.new(4)
    lcls.put 3, obj
    @cpu.literals = lcls
    
    @inst.run enc(:push_literal, 3)
    robj = obj_top
    
    assert_equal obj, robj
  end
  
  def test_push_self
    obj = Rubinius::Tuple.new(1)
    @cpu.self = obj
    
    @inst.run enc(:push_self)
    robj = obj_top
    assert_equal obj, robj
  end
  
  def test_goto
    bytes = enc(:goto, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    robj = obj_top
    assert robj.false?
  end
  
  def test_push_true
    @inst.run enc(:push_true)
    assert obj_top.true?
  end
  
  def test_push_false
    @inst.run enc(:push_false)
    assert obj_top.false?
  end
  
  def test_goto_if_false
    @cpu.stack_push CPU::FALSE
    bytes = enc(:goto_if_false, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_false_fails
    @cpu.stack_push CPU::TRUE
    bytes = enc(:goto_if_false, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_goto_if_false_with_nil
    @cpu.stack_push CPU::NIL
    bytes = enc(:goto_if_false, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
    
  def test_goto_if_true
    @cpu.stack_push CPU::TRUE
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_true_work_with_almost_anything
    @cpu.push_object RObject.wrap(0)
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_true_fails
    @cpu.stack_push CPU::FALSE
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_goto_if_true_fails_with_nil
    @cpu.stack_push CPU::NIL
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_goto_if_defined
    @cpu.stack_push CPU::TRUE
    bytes = enc(:goto_if_defined, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_defined_fails
    @cpu.stack_push CPU::UNDEF
    bytes = enc(:goto_if_defined, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    @inst.run bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_swap_stack
    @cpu.stack_push CPU::NIL
    @cpu.stack_push CPU::TRUE
    @inst.run enc(:swap_stack)
    assert_equal CPU::NIL, @cpu.stack_pop
    assert_equal CPU::TRUE, @cpu.stack_pop
  end
  
  def test_dup_top
    @cpu.stack_push CPU::TRUE
    @inst.run enc(:dup_top)
    assert_equal 1, @cpu.sp
    assert obj_top.true?
  end
  
  def test_pop
    @cpu.stack_push CPU::NIL
    @inst.run enc(:pop)
    assert @cpu.stack_empty?
  end
  
  def test_set_local
    lcls = Rubinius::Tuple.new(4)
    @cpu.locals = lcls
    @cpu.stack_push CPU::TRUE
    @inst.run enc(:set_local, 2)
    assert lcls.at(2).true?
    assert obj_top.true?
    assert_equal 0, @cpu.sp
  end
  
  def test_push_local
    lcls = Rubinius::Tuple.new(4)
    @cpu.locals = lcls
    lcls.put 2, RObject.true
    @inst.run enc(:push_local, 2)
    assert obj_top.true?
  end
  
  def test_push_exception
    obj = Rubinius::Tuple.new(3)
    @cpu.exception = obj
    @inst.run enc(:push_exception)
    assert_equal obj, obj_top
  end
  
  def test_make_array
    o1 = Rubinius::Tuple.new(3)
    o2 = Rubinius::Tuple.new(3)
    @cpu.push_object o1
    @cpu.push_object o2
    
    @inst.run enc(:make_array, 2)
    assert_equal 0, @cpu.sp
    a = obj_top
    a.as :array
    assert_equal CPU::Global.array, a.rclass
    assert_equal 2, a.total.to_int
    assert_equal o1, a.get(0)
    assert_equal o2, a.get(1)
  end
  
  def test_get_ivar
    ivar = Rubinius::String.new("@name")
    sym = ivar.to_sym
    tup = Rubinius::Tuple.new(3)
    obj = Rubinius::Object.new
    obj.set_ivar sym, tup
    @cpu.push_object obj
    @inst.run enc(:push_ivar, sym.symbol_index)
    assert_equal tup, obj_top
  end
  
  def test_set_ivar
    ivar = Rubinius::String.new("@name")
    sym = ivar.to_sym
    tup = Rubinius::Tuple.new(3)
    obj = Rubinius::Object.new
    @cpu.push_object obj
    @cpu.push_object tup
    @inst.run enc(:set_ivar, sym.symbol_index)
    out = obj.get_ivar sym
    assert_equal tup, out
    assert_equal 0, @cpu.sp
    assert_equal tup, obj_top
  end
  
  def test_push_const
    obj = Rubinius::Tuple.new(3)
    key = Rubinius::String.new("Test").to_sym
    hsh = CPU::Global.object.constants
    hsh.as :hash
    hsh.set key, obj
    @inst.run enc(:push_const, key.symbol_index)
    assert_equal obj, obj_top
  end
  
  def test_push_const_using_enclosing
    obj = Rubinius::Tuple.new(3)
    key = Rubinius::String.new("Test").to_sym
    @cpu.enclosing_class = CPU::Global.string
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    hsh.set key, obj
    @inst.run enc(:push_const, key.symbol_index)
    assert_equal obj, obj_top
  end
  
  def test_set_const
    obj = Rubinius::Tuple.new(3)
    key = Rubinius::String.new("Test").to_sym
    @cpu.push_object obj
    @inst.run enc(:set_const, key.symbol_index)
    assert_equal obj, obj_top
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    val = hsh.find(key)
    assert_equal obj, val
  end
  
  def test_find_const
    obj = Rubinius::Tuple.new(3)
    key = Rubinius::String.new("Test").to_sym
    cls = CPU::Global.string
    hsh = cls.constants
    hsh.as :hash
    hsh.set key, obj
    @cpu.push_object cls
    @inst.run enc(:find_const, key.symbol_index)
    assert_equal obj, obj_top
  end
  
  def test_push_block
    obj = s_tup()
    
    @cpu.block = obj
    @inst.run enc(:push_block)
    assert_equal obj, obj_top
  end
  
  def test_send_stack
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    bc =  enc(:push_true)
    bc << enc(:set_const, key.symbol_index)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    lits = Rubinius::Tuple.new(1)
    lits.put 0, mkey
    
    mtbl.set mkey, mth
    @cpu.push_object obj
    ctx = @cpu.active_context
    ctx.as :methctx
    ctx.literals = lits
    
    @cpu.execute_bytecodes enc(:send_stack, 0)
    assert obj_top.true?
    
    ctbl = CPU::Global.object.constants
    ctbl.as :hash
    out = ctbl.find(key)
    assert out.true?
  end
  
  def test_send_stack_no_method_missing
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    bc =  enc(:push_true)
    bc << enc(:set_const, key.symbol_index)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    lits = Rubinius::Tuple.new(1)
    lits.put 0, mkey
    ctx = @cpu.active_context
    ctx.as :methctx
    
    ctx.literals = lits
    
    # mtbl.set mkey, mth
    @cpu.push_object obj
    assert_raises(RuntimeError) do
      @cpu.execute_bytecodes enc(:send_stack, 0)
    end
  end
  
  def test_send_stack_method_missing
    key =  sym("Name")
    kkey = sym("method_missing")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    bc =  enc(:push_true)
    bc << enc(:set_const, key.symbol_index)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    mtbl.set kkey, mth
    ctbl = CPU::Global.object.constants
    ctbl.as :hash
    
    lits = Rubinius::Tuple.new(1)
    lits.put 0, mkey
    ctx = @cpu.active_context
    ctx.as :methctx
    
    ctx.literals = lits
    
    assert ctbl.find(key).nil?
    @cpu.push_object obj
    @cpu.execute_bytecodes enc(:send_stack, 0)
    assert obj_top.true?

    out = ctbl.find(key)
    assert out.true?
  end
  
  def test_clear_exception
    obj = s_tup()
    @cpu.exception = obj
    @inst.run enc(:clear_exception)
    assert @cpu.exception.nil?
  end
  
  def sym(str)
    Rubinius::String.new(str).to_sym
  end
  
  def s_tup
    Rubinius::Tuple.new(3)
  end
  
  def test_attach_method
    key = sym("name")
    obj = s_tup
    meth = s_tup
    
    @cpu.push_object meth
    @cpu.push_object obj
    @inst.run enc(:attach_method, key.symbol_index)
    assert_equal meth, obj_top
    
    meths = obj.metaclass.methods
    meths.as :methtbl
    
    out = meths.find(key)
    assert_equal out, meth
  end
  
  def test_add_method
    key = sym("name")
    obj = s_tup()
    meth = s_tup()
    
    @cpu.push_object meth
    @cpu.push_object CPU::Global.object
    @inst.run enc(:add_method, key.symbol_index)
    assert_equal meth, obj_top
    
    meths = CPU::Global.object.methods
    meths.as :methtbl
    
    out = meths.find(key)
    assert_equal out, meth
  end
  
  def test_open_class_no_super
    key = sym("EEEK")
    sup = RObject.nil
    
    @cpu.push_object sup
    @inst.run enc(:open_class, key.symbol_index)
    cls = obj_top
    cls.as :class
    
    assert_equal CPU::Global.class, cls.rclass
    assert_equal CPU::Global.object, cls.superclass
  end
  
  def test_open_class_with_super
    key = sym("EEEK")
    sup = CPU::Global.class
    
    @cpu.push_object sup
    @inst.run enc(:open_class, key.symbol_index)
    cls = obj_top
    cls.as :class
    
    assert_equal CPU::Global.class, cls.rclass
    assert_equal CPU::Global.class, cls.superclass
  end
  
  # We don't support this yet because there is no reason yet.
  def notest_open_class_from_cli
    key = sym("EEEK")
    key2 = sym("EEK2")
    sup = CPU::Global.class
    
    @cpu.push_object sup
    @inst.run enc(:open_class, key.symbol_index)
    cls = obj_top
    cls.as :class
    
    @cpu.new_class_of = cls
    
    @cpu.push_object sup
    @inst.run enc(:open_class, key2.symbol_index)
    cls2 = obj_top
    cls2.as :class
    
    p cls
    p cls2
    p CPU::Global.class
    p cls2.rclass
    
    assert_equal cls.address, cls2.rclass.address
  end
  
  def test_open_class_existing
    key = sym("EEEK")
    sup = RObject.nil
    
    @cpu.push_object sup
    @inst.run enc(:open_class, key.symbol_index)
    cls = obj_top
    
    @cpu.push_object sup
    @inst.run enc(:open_class, key.symbol_index)
    cls2 = obj_top
    
    assert_equal cls, cls2
  end 
  
  def test_open_class_existing_bad_super
    key = sym("EEEK")
    sup = RObject.nil
    
    @cpu.push_object sup
    @inst.run enc(:open_class, key.symbol_index)
    cls = obj_top
    
    @cpu.push_object CPU::Global.class
    @inst.run enc(:open_class, key.symbol_index)
    assert obj_top.nil?    
  end
  
  def test_soft_return
    @inst.run enc(:soft_return)
    assert @cpu.active_context.nil?
  end
  
  def test_soft_return_into_block
    ctx = @cpu.active_context
    b1 = Rubinius::BlockContext.under_context ctx
    b1.ip = RObject.wrap(3)
    b2 = Rubinius::BlockContext.under_context ctx
    b2.ip = RObject.wrap(3)
    ctx.ip = RObject.wrap(3)
    b2.sender = b1
    b1.sender = ctx
    @cpu.active_context = b2
    @inst.run enc(:soft_return)
    assert @cpu.active_context
    assert_equal b1, @cpu.active_context
    assert_equal ctx, @cpu.home_context
    
    @inst.run enc(:soft_return)
    assert @cpu.active_context
    assert_equal ctx, @cpu.active_context
    assert_equal ctx, @cpu.home_context
  end
  
  def test_caller_return
    ctx = @cpu.active_context
    c2 = Rubinius::MethodContext.new(0)
    b1 = Rubinius::BlockContext.under_context ctx
    c2.sender = ctx
    b1.sender = c2
    @cpu.active_context = b1
    @inst.caller_return
    assert @cpu.active_context
    assert_equal ctx, @cpu.active_context
  end
end

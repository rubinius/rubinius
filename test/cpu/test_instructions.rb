require 'rubygems'
require 'test/unit'
require 'cpu/instructions'
require 'cpu/runtime'
require 'bytecode/encoder'


class TestCPUInstructions < Test::Unit::TestCase
  
  def setup
    @encoder = Bytecode::InstructionEncoder.new
    @cpu = CPU.new
    @inst = @cpu
    # @inst = CPU::Instructions.new(@cpu)
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
  
  def run_bytes(bytes)
    @cpu.run_bytes bytes
  end
  
  def test_run
    params = enc(:push_int, 1) + enc(:push_int, 2) + enc(:push_int, 100)
    run_bytes params
    assert_equal 2, @cpu.sp
  end
    
  def test_noop
    run_bytes enc(:noop)
    assert_equal 1, @cpu.ip
  end
  
  def test_push_nil
    run_bytes enc(:push_nil)
    assert_equal 1, @cpu.ip
    assert_equal 0, @cpu.sp
    assert_equal CPU::NIL, @cpu.stack_top
  end
  
  def test_allocate
    run_bytes enc(:allocate, 10)
    assert !@cpu.stack_empty?
    obj = RObject.new(@cpu.stack_top)
    assert_equal 10, obj.fields
    assert obj.rclass.nil?
  end
  
  def test_set_class
    run_bytes enc(:allocate, 4)
    klass = @cpu.stack_top
    run_bytes enc(:allocate, 4)
    obj = @cpu.stack_top
    csp = @cpu.sp
    run_bytes enc(:set_class)
    
    robj = RObject.new(obj)
    assert_equal csp - 1, @cpu.sp
    assert_equal klass, robj.rclass.address
    assert_equal obj, @cpu.stack_top
  end
  
  def test_push_int
    run_bytes enc(:push_int, 88)
    robj = RObject.new @cpu.stack_top
    assert robj.fixnum?
    assert_equal 88, robj.to_int
  end
  
  def test_store_field
    run_bytes enc(:allocate, 4)
    obj = @cpu.stack_top
    run_bytes enc(:push_int, 88)
    run_bytes enc(:push_int, 1)
    run_bytes enc(:store_field)
    robj = RObject.new @cpu.stack_top
    assert_equal obj, robj.address
    
    int = robj.at(1)
    assert int.fixnum?
    assert_equal 88, int.to_int
  end
  
  def test_store_field_at
    run_bytes enc(:allocate, 4)
    run_bytes enc(:push_int, 89)
    run_bytes enc(:store_field_at, 1)
    
    robj = RObject.new @cpu.stack_top
    int = robj.at(1)
    assert_equal 89, int.to_int
  end
  
  def obj_top
    RObject.new @cpu.stack_top
  end
  
  def test_fetch_field
    fel = 2
    run_bytes enc(:allocate, 4)
    run_bytes enc(:push_int, 99)
    run_bytes enc(:push_int, fel)
    run_bytes enc(:store_field)
    run_bytes enc(:push_int, fel)
    run_bytes enc(:fetch_field)
    
    robj = obj_top
    
    assert_equal 99, robj.to_int
  end
  
  def test_send_primitive
    run_bytes enc(:allocate, 4)
    run_bytes enc(:send_primitive, 0)
    assert @cpu.stack_empty?
  end
  
  def test_push_context
    run_bytes enc(:push_context)
    assert_equal @cpu.active_context, obj_top
  end
  
  def test_push_literal
    obj = Rubinius::Tuple.new(1)
    lcls = Rubinius::Tuple.new(4)
    lcls.put 3, obj
    @cpu.literals = lcls
    
    run_bytes enc(:push_literal, 3)
    robj = obj_top
    
    assert_equal obj, robj
  end
  
  def test_push_self
    obj = Rubinius::Tuple.new(1)
    @cpu.self = obj
    
    run_bytes enc(:push_self)
    robj = obj_top
    assert_equal obj, robj
  end
  
  def test_goto
    bytes = enc(:goto, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    robj = obj_top
    assert robj.false?
  end
  
  def test_push_true
    run_bytes enc(:push_true)
    assert obj_top.true?
  end
  
  def test_push_false
    run_bytes enc(:push_false)
    assert obj_top.false?
  end
  
  def test_goto_if_false
    @cpu.stack_push CPU::FALSE
    bytes = enc(:goto_if_false, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_false_fails
    @cpu.stack_push CPU::TRUE
    bytes = enc(:goto_if_false, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_goto_if_false_with_nil
    @cpu.stack_push CPU::NIL
    bytes = enc(:goto_if_false, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
    
  def test_goto_if_true
    @cpu.stack_push CPU::TRUE
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_true_work_with_almost_anything
    @cpu.push_object RObject.wrap(0)
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_true_fails
    @cpu.stack_push CPU::FALSE
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_goto_if_true_fails_with_nil
    @cpu.stack_push CPU::NIL
    bytes = enc(:goto_if_true, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_goto_if_defined
    @cpu.stack_push CPU::TRUE
    bytes = enc(:goto_if_defined, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 0, @cpu.sp
    assert obj_top.false?
  end
  
  def test_goto_if_defined_fails
    @cpu.stack_push CPU::UNDEF
    bytes = enc(:goto_if_defined, 6)
    bytes << enc(:push_true)
    bytes << enc(:push_false)
    run_bytes bytes
    assert_equal 1, @cpu.sp
    assert @cpu.stack.at(0).true?
    assert obj_top.false?
  end
  
  def test_swap_stack
    @cpu.stack_push CPU::NIL
    @cpu.stack_push CPU::TRUE
    run_bytes enc(:swap_stack)
    assert_equal CPU::NIL, @cpu.stack_pop
    assert_equal CPU::TRUE, @cpu.stack_pop
  end
  
  def test_dup_top
    @cpu.stack_push CPU::TRUE
    run_bytes enc(:dup_top)
    assert_equal 1, @cpu.sp
    assert obj_top.true?
  end
  
  def test_pop
    @cpu.stack_push CPU::NIL
    run_bytes enc(:pop)
    assert @cpu.stack_empty?
  end
  
  def test_set_local
    lcls = Rubinius::Tuple.new(4)
    @cpu.locals = lcls
    @cpu.stack_push CPU::TRUE
    run_bytes enc(:set_local, 2)
    assert lcls.at(2).true?
    assert @cpu.stack_empty?
  end
  
  def test_push_local
    lcls = Rubinius::Tuple.new(4)
    @cpu.locals = lcls
    lcls.put 2, RObject.true
    run_bytes enc(:push_local, 2)
    assert obj_top.true?
  end
  
  def test_push_exception
    obj = Rubinius::Tuple.new(3)
    @cpu.exception = obj
    run_bytes enc(:push_exception)
    assert_equal obj, obj_top
  end
  
  def test_make_array
    o1 = Rubinius::Tuple.new(3)
    o2 = Rubinius::Tuple.new(3)
    @cpu.push_object o1
    @cpu.push_object o2
    
    run_bytes enc(:make_array, 2)
    assert_equal 0, @cpu.sp
    a = obj_top
    a.as :array
    assert_equal CPU::Global.array, a.rclass
    assert_equal 2, a.total.to_int
    assert_equal o1, a.get(0)
    assert_equal o2, a.get(1)
  end
  
  def test_push_array
    ary = Rubinius::Array.new(2)
    ary.set 0, RObject.wrap(99)
    ary.set 1, RObject.wrap(88)
    
    @cpu.push_object ary
    run_bytes enc(:push_array)
    assert_equal 1, @cpu.sp
    assert_equal 99, @cpu.pop_object.to_int
    assert_equal 88, @cpu.pop_object.to_int
  end
  
  def test_cast_array_into_array
    ary = Rubinius::Array.new(2)
    ary.set 0, RObject.wrap(99)
    ary.set 1, RObject.wrap(88)
    
    @cpu.push_object ary
    run_bytes enc(:cast_array)
    assert_equal ary.address, @cpu.stack_pop
  end
  
  def test_cast_array
    @cpu.push_object RObject.wrap(99)
    run_bytes enc(:cast_array)
    ary = @cpu.pop_object
    ary.as :array
    assert_equal CPU::Global.array, ary.rclass
    assert_equal 1, ary.total.to_int
    assert_equal 99, ary.get(0).to_int
  end
  
  def test_cast_array_for_args
    @cpu.push_object RObject.wrap(99)
    run_bytes enc(:cast_array_for_args, 3)
    ary = @cpu.pop_object
    ary.as :array
    assert_equal CPU::Global.array, ary.rclass
    assert_equal 1, ary.total.to_int
    assert_equal 99, ary.get(0).to_int
    assert_equal 4, @cpu.args
  end
  
  
  def test_cast_tuple_from_tuple
    tup = Rubinius::Tuple.new(1)
    @cpu.push_object tup
    run_bytes enc(:cast_tuple)
    assert_equal tup.address, @cpu.stack_pop
  end
  
  def test_cast_tuple
    obj = RObject.wrap(99)
    @cpu.push_object obj
    run_bytes enc(:cast_tuple)
    tup = @cpu.pop_object
    assert_equal 99, tup.at(0).to_int
  end
    
  def test_cast_tuple_into_array
    tup = Rubinius::Tuple.new(1)
    tup.put 0, RObject.wrap(101)
    @cpu.push_object tup
    run_bytes enc(:cast_array)
    ary = @cpu.pop_object
    ary.as :array
    assert_equal CPU::Global.array.address, ary.rclass.address
    assert_equal 1, ary.total.to_int
    assert_equal 101, ary.get(0).to_int
  end
  
  def test_make_hash
    o1 = RObject.wrap(99)
    o2 = Rubinius::Tuple.new(3)
    @cpu.push_object o2
    @cpu.push_object o1
    
    run_bytes enc(:make_hash, 2)
    assert_equal 0, @cpu.sp
    a = obj_top
    a.as :hash
    assert_equal CPU::Global.hash, a.rclass
    assert_equal o2, a.find(o1)
  end
  
  def test_get_ivar
    ivar = Rubinius::String.new("@name")
    sym = ivar.to_sym
    tup = Rubinius::Tuple.new(3)
    obj = Rubinius::Object.new
    obj.set_ivar sym, tup
    @cpu.self = obj
    add_sym_lit "@name"
    
    run_bytes enc(:push_ivar, 0)
    assert_equal tup, obj_top
  end
  
  def test_set_ivar
    ivar = Rubinius::String.new("@name")
    sym = ivar.to_sym
    tup = Rubinius::Tuple.new(3)
    obj = Rubinius::Object.new
    @cpu.self = obj
    # @cpu.push_object obj
    @cpu.push_object tup
    add_sym_lit "@name"
    run_bytes enc(:set_ivar, 0)
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
    add_sym_lit "Test"
    run_bytes enc(:push_const, 0)
    assert_equal obj, obj_top
  end
  
  def test_push_const_using_enclosing
    obj = Rubinius::Tuple.new(3)
    key = Rubinius::String.new("Test").to_sym
    @cpu.enclosing_class = CPU::Global.string
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    hsh.set key, obj
    add_sym_lit "Test"
    run_bytes enc(:push_const, 0)
    assert_equal obj, obj_top
  end
  
  def test_set_const
    obj = Rubinius::Tuple.new(3)
    key = Rubinius::String.new("Test").to_sym
    @cpu.push_object obj
    add_sym_lit "Test"
    run_bytes enc(:set_const, 0)
    assert_equal obj, obj_top
    hsh = @cpu.enclosing_class.constants
    hsh.as :hash
    val = hsh.find(key)
    assert_equal obj, val
  end
  
  def test_set_const_at
    obj = Rubinius::Tuple.new(3)
    key = Rubinius::String.new("Test").to_sym
    @cpu.push_object CPU::Global.class
    @cpu.push_object obj
    add_sym_lit "Test"
    run_bytes enc(:set_const_at, 0)
    assert_equal obj, obj_top
    hsh = CPU::Global.class.constants
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
    add_sym_lit "Test"
    run_bytes enc(:find_const, 0)
    assert_equal obj, obj_top
  end
  
  def test_push_cpath_top
    run_bytes enc(:push_cpath_top)
    assert_equal CPU::Global.object, obj_top
  end
  
  def test_push_block
    obj = s_tup()
    
    @cpu.block = obj
    run_bytes enc(:push_block)
    assert_equal obj, obj_top
  end
  
  def add_sym_lit(name)
    s = sym(name)
    lits = Rubinius::Tuple.new(4)
    lits.put 0, s
    @cpu.literals = lits
  end
  
  def test_activate_method
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    bc =  enc(:push_true)
    bc << enc(:set_const, 1)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    
    lits = Rubinius::Tuple.new(2)
    lits.put 0, mkey
    lits.put 1, key
    
    mth.literals = lits

    @cpu.push_object RObject.wrap(0)
    @cpu.push_object mth
    @cpu.push_object obj
    ctx = @cpu.active_context
    @cpu.execute_bytecodes enc(:activate_method, 0)
    assert obj_top.true?
    
    ctbl = CPU::Global.object.constants
    ctbl.as :hash
    out = ctbl.find(key)
    assert out.true?
  end
  
  def test_send_method
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    bc =  enc(:push_true)
    bc << enc(:set_const, 1)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    lits = Rubinius::Tuple.new(2)
    lits.put 0, mkey
    lits.put 1, key
    
    mth.literals = lits
    
    mtbl.set mkey, mth
    @cpu.push_object obj
    ctx = @cpu.active_context
    ctx.as :methctx
    ctx.literals = lits
    
    @cpu.execute_bytecodes enc(:send_method, 0)
    assert obj_top.true?
    
    ctbl = CPU::Global.object.constants
    ctbl.as :hash
    out = ctbl.find(key)
    assert out.true?
  end
  
  def test_send_method_no_method_missing
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    bc =  enc(:push_true)
    add_sym_lit "Name"
    bc << enc(:set_const, 0)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    lits = Rubinius::Tuple.new(2)
    lits.put 0, mkey
    lits.put 1, key
    
    ctx = @cpu.active_context
    ctx.as :methctx
    
    mth.literals = lits
    ctx.literals = lits
    
    # mtbl.set mkey, mth
    @cpu.push_object obj
    assert_raises(RuntimeError) do
      @cpu.execute_bytecodes enc(:send_method, 0)
    end
  end
  
  def test_send_method_method_missing
    key =  sym("Name")
    kkey = sym("method_missing")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    bc =  enc(:push_true)
    add_sym_lit "Name"
    bc << enc(:set_const, 1)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    mtbl.set kkey, mth
    ctbl = CPU::Global.object.constants
    ctbl.as :hash
    
    lits = Rubinius::Tuple.new(2)
    lits.put 0, mkey
    lits.put 1, key
    ctx = @cpu.active_context
    ctx.as :methctx
    
    mth.literals = lits
    ctx.literals = lits
    
    assert ctbl.find(key).nil?
    @cpu.push_object obj
    @cpu.execute_bytecodes enc(:send_method, 0)
    assert obj_top.true?

    out = ctbl.find(key)
    assert out.true?
  end
  
  def test_send_stack_with_block
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    add_sym_lit "Name"
    bc = enc(:push_block)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    lits = Rubinius::Tuple.new(2)
    lits.put 0, mkey
    lits.put 1, key
    
    mtbl.set mkey, mth
    ctx = @cpu.active_context
    ctx.as :methctx
    ctx.literals = lits
    mth.literals = lits
    
    @cpu.push_object RObject.true
    @cpu.push_object obj
    
    @cpu.execute_bytecodes enc(:send_stack_with_block, 0, 0)
    assert obj_top.true?    
  end
  
  def test_send_stack
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    add_sym_lit "Name"
    bc = enc(:set_const, 1)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    lits = Rubinius::Tuple.new(2)
    lits.put 0, mkey
    lits.put 1, key
    
    mtbl.set mkey, mth
    ctx = @cpu.active_context
    ctx.as :methctx
    ctx.literals = lits
    mth.literals = lits
    
    @cpu.push_object RObject.true
    @cpu.push_object obj
    
    @cpu.execute_bytecodes enc(:send_stack, 0, 1)
    assert obj_top.true?
    
    ctbl = CPU::Global.object.constants
    ctbl.as :hash
    out = ctbl.find(key)
    assert out.true?
  end
  
  def test_send_with_arg_register
    key =  sym("Name")
    mkey = sym("test_method")
    obj = Rubinius::Object.new
    add_sym_lit "Name"
    bc = enc(:set_const, 1)
    mth = Rubinius::CompiledMethod.from_string bc, 0
    mtbl = CPU::Global.object.methods
    mtbl.as :methtbl
    
    lits = Rubinius::Tuple.new(2)
    lits.put 0, mkey
    lits.put 1, key
    
    mtbl.set mkey, mth
    ctx = @cpu.active_context
    ctx.as :methctx
    ctx.literals = lits
    mth.literals = lits
    
    @cpu.push_object RObject.true
    @cpu.push_object RObject.nil
    @cpu.push_object obj
    @cpu.push_object RObject.wrap(1)
    
    @cpu.execute_bytecodes enc(:set_args) + enc(:send_with_arg_register, 0)
    assert obj_top.true?
    
    ctbl = CPU::Global.object.constants
    ctbl.as :hash
    out = ctbl.find(key)
    assert out.true?
  end
  
  def test_clear_exception
    obj = s_tup()
    @cpu.exception = obj
    run_bytes enc(:clear_exception)
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
    add_sym_lit "name"
    run_bytes enc(:attach_method, 0)
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
    add_sym_lit "name"
    run_bytes enc(:add_method, 0)
    assert_equal meth, obj_top
    
    meths = CPU::Global.object.methods
    meths.as :methtbl
    
    out = meths.find(key)
    assert_equal out, meth
  end
  
  def test_open_module
    add_sym_lit "EEEK"
    run_bytes enc(:open_module, 0)
    cls = obj_top
    cls.as :module
    
    assert_equal CPU::Global.module, cls.rclass
  end
  
  def test_open_module_existing
    add_sym_lit "EEEK"
    run_bytes enc(:open_module, 0)
    cur = obj_top
    run_bytes enc(:open_module, 0)
    mod = obj_top
    
    assert_equal cur, mod
  end
  
  def test_open_module_under
    add_sym_lit "EEEK"
    @cpu.push_object CPU::Global.class
    run_bytes enc(:open_module_under, 0)
    mod = obj_top()
    hsh = CPU::Global.class.constants
    hsh.as :hash
    key = Rubinius::String.new("EEEK").to_sym
    out = hsh.find(key)
    assert_equal mod, out
  end
  
  def test_open_class_under
    add_sym_lit "EEEK"
    @cpu.push_object CPU::Global.class
    @cpu.push_object RObject.nil
    run_bytes enc(:open_class_under, 0)
    mod = obj_top()
    hsh = CPU::Global.class.constants
    hsh.as :hash
    key = Rubinius::String.new("EEEK").to_sym
    out = hsh.find(key)
    assert_equal mod, out
  end
  
  def test_open_class_no_super
    key = sym("EEEK")
    sup = RObject.nil
    
    add_sym_lit "EEEK"
    @cpu.push_object sup
    run_bytes enc(:open_class, 0)
    cls = obj_top
    cls.as :class
    
    assert_equal CPU::Global.class, cls.rclass
    assert_equal CPU::Global.object, cls.superclass
  end
  
  def test_open_class_with_super
    key = sym("EEEK")
    sup = CPU::Global.class
    
    @cpu.push_object sup
    add_sym_lit "EEEK"
    run_bytes enc(:open_class, 0)
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
    add_sym_lit "EEEK"
    run_bytes enc(:open_class, 0)
    cls = obj_top
    cls.as :class
    
    @cpu.new_class_of = cls
    
    @cpu.push_object sup
    @cpu.literals.put 1, key2
    run_bytes enc(:open_class, 1)
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
    add_sym_lit "EEEK"
    run_bytes enc(:open_class, 0)
    cls = obj_top
    
    @cpu.push_object sup
    run_bytes enc(:open_class, 0)
    cls2 = obj_top
    
    assert_equal cls, cls2
  end 
  
  def test_open_class_existing_bad_super
    key = sym("EEEK")
    sup = RObject.nil
    
    @cpu.push_object sup
    add_sym_lit "EEEK"
    run_bytes enc(:open_class, 0)
    cls = obj_top
    
    @cpu.push_object CPU::Global.class
    run_bytes enc(:open_class, 0)
    assert obj_top.nil?    
  end
  
  def test_soft_return
    run_bytes enc(:soft_return)
    assert @cpu.active_context.nil?
  end
  
  def test_soft_return_into_block
    ctx = @cpu.active_context
    env = Rubinius::BlockEnvironment.under_context ctx, RObject.wrap(0)
    b1 = env.create_context
    b1.ip = RObject.wrap(3)
    
    env = Rubinius::BlockEnvironment.under_context ctx, RObject.wrap(0)
    b2 = env.create_context
    b2.ip = RObject.wrap(3)
    ctx.ip = RObject.wrap(3)
    b2.sender = b1
    b1.sender = ctx
    @cpu.active_context = b2
    run_bytes enc(:soft_return)
    assert @cpu.active_context
    assert_equal b1, @cpu.active_context
    assert_equal ctx, @cpu.home_context
    
    run_bytes enc(:soft_return)
    assert @cpu.active_context
    assert_equal ctx, @cpu.active_context
    assert_equal ctx, @cpu.home_context
  end
  
  def test_caller_return
    ctx = @cpu.active_context
    c2 = Rubinius::MethodContext.new(0)
    env = Rubinius::BlockEnvironment.under_context ctx, RObject.wrap(0)
    b1 = env.create_context
    c2.sender = ctx
    b1.sender = c2
    @cpu.active_context = b1
    @inst.caller_return
    assert @cpu.active_context
    assert_equal ctx, @cpu.active_context
  end
  
  def test_raise_exc
    exc = Rubinius::Tuple.new(1)
    ent = Rubinius::Tuple.new(3)
    ent.put 0, RObject.wrap(1)
    ent.put 1, RObject.wrap(1)
    ent.put 2, RObject.wrap(6)
    exc.put 0, ent
    
    tup = Rubinius::Tuple.new(1)
    
    @cpu.push_object tup
    @cpu.active_context.raiseable = RObject.true
    bc = enc(:raise_exc) + enc(:goto, 7) + enc(:push_exception) + enc(:push_true)
    @cpu.exceptions = exc
    run_bytes bc
    assert_equal 1, @cpu.sp
    assert @cpu.pop_object.true?
    assert_equal tup, @cpu.pop_object
  end
  
  def test_raise_exc_not_raiseable
    tup = Rubinius::Tuple.new(1)
    
    @cpu.push_object tup
    bc = enc(:raise_exc)
    @cpu.active_context.raiseable = RObject.false
    run_bytes bc
    assert_equal tup, @cpu.exception
  end
  
  def test_unshift_tuple
    tup = Rubinius::Tuple.new(2)
    tup.put 0, RObject.wrap(99)
    tup.put 1, RObject.wrap(101)
    
    @cpu.push_object tup
    bc = enc(:unshift_tuple)
    run_bytes bc
    assert_equal 1, @cpu.sp
    assert_equal 99, @cpu.pop_object.to_int
    t2 = @cpu.pop_object
    t2.as :tuple
    assert_equal 1, t2.fields
    assert_equal 101, t2.at(0).to_int
  end
  
  def test_unshift_tuple_get_nil
    tup = Rubinius::Tuple.new(0)
    @cpu.push_object tup
    bc = enc(:unshift_tuple)
    run_bytes bc
    assert @cpu.pop_object.nil?
    assert_equal tup, @cpu.pop_object
  end
  
  def test_make_rest
    @cpu.active_context.argcount = RObject.wrap(3)
    @cpu.push_object RObject.wrap(333)
    @cpu.push_object RObject.wrap(99)
    
    run_bytes enc(:make_rest, 1)
    
    assert_equal 0, @cpu.sp
    ary = @cpu.pop_object
    
    assert_equal CPU::Global.array.address, ary.rclass.address
    ary.as :array
    
    assert_equal 2, ary.total.to_int
    assert_equal 99, ary.get(0).to_int
    assert_equal 333, ary.get(1).to_int
  end
  
  def test_set_encloser
    @cpu.push_object CPU::Global.class
    run_bytes enc(:set_encloser)
    assert_equal CPU::Global.class, @cpu.enclosing_class
  end
  
  def test_push_encloser
    run_bytes enc(:push_encloser)
    assert_equal @cpu.enclosing_class, @cpu.pop_object
  end
  
  def test_check_argcount_fixed_only
    @cpu.argcount = 2
    @cpu.active_context.raiseable = RObject.false
    run_bytes enc(:check_argcount, 2, 2)
    assert @cpu.exception.nil?
  end
  
  def test_check_argcount_too_few
    @cpu.argcount = 1
    @cpu.active_context.raiseable = RObject.false
    run_bytes enc(:check_argcount, 2, 2)
    assert_equal CPU::Global.exc_arg, @cpu.exception.rclass
    msg = @cpu.exception.at(0).as(:string).as_string
    assert_equal "wrong number of arguments (1 for 2)", msg
  end
  
  def test_check_argcount_too_many
    @cpu.argcount = 3
    @cpu.active_context.raiseable = RObject.false
    run_bytes enc(:check_argcount, 2, 2)
    assert_equal CPU::Global.exc_arg, @cpu.exception.rclass
    msg = @cpu.exception.at(0).as(:string).as_string
    assert_equal "wrong number of arguments (3 for 2)", msg
  end
  
  def test_check_argcount_with_splat
    @cpu.argcount = 10
    @cpu.active_context.raiseable = RObject.false
    run_bytes enc(:check_argcount, 2, 0)
    assert @cpu.exception.nil?
  end
  
  def test_passed_arg
    @cpu.argcount = 2
    run_bytes enc(:passed_arg, 0)
    assert @cpu.pop_object.true?
    run_bytes enc(:passed_arg, 1)
    assert @cpu.pop_object.true?
    run_bytes enc(:passed_arg, 2)
    assert @cpu.pop_object.false?
    run_bytes enc(:passed_arg, 10000)
    assert @cpu.pop_object.false?
  end
  
  def test_string_append
    str = Rubinius::String.new("hello")
    str2 = Rubinius::String.new(" world")
    str3 = Rubinius::String.new(", everyone.")
    @cpu.push_object str3
    @cpu.push_object str2
    @cpu.push_object str
    run_bytes enc(:string_append) + enc(:string_append)
    out = @cpu.pop_object
    out.as :string
    assert_equal "hello world, everyone.", out.as_string
    assert_equal str.address, out.address
  end
  
  def test_string_dup
    str = Rubinius::String.new("hello")
    @cpu.push_object str
    run_bytes enc(:string_dup)
    out = @cpu.pop_object
    out.as :string
    assert_equal "hello", out.as_string
    assert str.address != out.address
  end
  
  def test_set_args
    @cpu.push_object RObject.wrap(99)
    run_bytes enc(:set_args)
    assert_equal 99, @cpu.args
  end
  
  def test_get_args
    @cpu.args = 33
    run_bytes enc(:get_args)
    assert_equal 33, @cpu.pop_object.to_int
  end
  
end

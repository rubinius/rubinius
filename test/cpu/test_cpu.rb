require 'rubygems'
require 'test/unit'
require 'cpu/instructions'
require 'cpu/runtime'
require 'cpu/encoder'
require 'test/unit/show_code'

class TestCPURuntime < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_ip
    assert_equal 0, @cpu.ip
  end
  
  def test_sp
    assert_equal -1, @cpu.sp
    @cpu.stack_push 4
    assert_equal 0, @cpu.sp
  end
  
  def test_stack_push_and_pop
    @cpu.stack_push 4
    addr = @cpu.stack_pop
    assert_equal 4, addr
  end
  
  def test_stack_pop_off_end
    assert_raises(RuntimeError) do
      @cpu.stack_pop
    end
  end
  
  def test_stack_top
    @cpu.stack_push 10
    assert_equal 10, @cpu.stack_top
  end
  
  def test_stack_empty?
    assert @cpu.stack_empty?
    @cpu.stack_push 10
    assert !@cpu.stack_empty?
  end
  
  def test_stack_top_off_end
    assert_raises(RuntimeError) do
      @cpu.stack_top
    end
  end
  
  def test_local_get
    @cpu.push_object RObject.wrap(99)
    @cpu.push_object RObject.wrap(4323)
    
    one = RObject.new @cpu.local_get(1)
    two = RObject.new @cpu.local_get(2)
    
    assert_equal 99, one.to_int
    assert_equal 4323, two.to_int
  end
  
  def test_local_set
    @cpu.local_set 2, RObject.wrap(832).address
    oop = Memory.fetch_long @cpu.stack.field_address(@cpu.ms + 1), 0
    obj = RObject.new oop
    assert_equal 832, obj.to_int
  end
  
  def test_save_registers
    @cpu.bootstrap
    
    @cpu.sp = 99
    @cpu.ip = 123
    @cpu.ms = 88
    
    ac = @cpu.active_context
    assert_equal 0, ac.sp.to_int
    assert_equal 0, ac.ip.to_int
    assert_equal 0, ac.ms.to_int    
    
    @cpu.save_registers

    assert_equal 99, ac.sp.to_int
    assert_equal 123, ac.ip.to_int
    assert_equal 88, ac.ms.to_int
  end
  
  def test_restore_registers
    ctx = Rubinius::MethodContext.new_anonymous
    ctx.ip = RObject.wrap(88)
    ctx.sp = RObject.wrap(33)
    ctx.ms = RObject.wrap(22)
    
    @cpu.restore_context ctx
    
    assert_equal 88, @cpu.ip
    assert_equal 33, @cpu.sp
    assert_equal 22, @cpu.ms
  end
  
  def test_metaclass
    @cpu.bootstrap
    
    obj = Rubinius::Class.create
    cls = Rubinius::Class.create
    
    assert obj.metaclass.nil?
    assert cls.metaclass.nil?
    
    obj.create_metaclass(cls)
    m1 = obj.metaclass
    assert m1.reference?
    assert_equal cls, m1.superclass
    assert_equal m1, obj.direct_class
    assert_equal cls.address, obj.rclass.address
  end
  
  def test_bootstrap
    @cpu.bootstrap
    
    cls = CPU::Global.class
    obj = CPU::Global.object
    mod = CPU::Global.module
    
    assert_equal cls, cls.rclass
    assert_equal cls, obj.rclass
    assert_equal cls, mod.rclass
    
    assert_equal mod, cls.superclass
    assert_equal obj, mod.superclass
    assert obj.superclass.nil?
    
    assert_equal cls.fields, Rubinius::Class::NumberOfFields
    assert_equal mod.fields, Rubinius::Class::NumberOfFields
    assert_equal obj.fields, Rubinius::Class::NumberOfFields

    assert_equal cls.instance_fields.to_int, Rubinius::Class::NumberOfFields    
    assert_equal mod.instance_fields.to_int, Rubinius::Module::NumberOfFields
    assert_equal obj.instance_fields.to_int, Rubinius::Object::NumberOfFields
    
    met = CPU::Global.metaclass
    
    assert_equal cls, met.rclass
    assert_equal cls, met.superclass
    assert_equal met.instance_fields.to_int, Rubinius::MetaClass::NumberOfFields
    
    cls_meta = cls.metaclass
    mod_meta = mod.metaclass
    obj_meta = obj.metaclass
    
    assert cls_meta.reference?
    assert mod_meta.reference?
    assert obj_meta.reference?
    
    assert_equal mod_meta, cls_meta.superclass
    assert_equal obj_meta, mod_meta.superclass
    assert_equal cls, obj_meta.superclass
  end
  
  def test_bootstrap_secondary
    @cpu.bootstrap
    
    obj = CPU::Global.object
    sym = CPU::Global.symbol
    tup = CPU::Global.tuple
    
    assert_equal obj, sym.superclass
    assert_equal 0, sym.instance_fields.to_int
    
    assert_equal obj, tup.superclass
    assert_equal 0, tup.instance_fields.to_int
    
    obj_meta = obj.metaclass
    tup_meta = tup.metaclass
    
    assert obj_meta.reference?
    assert_equal obj_meta.address, tup_meta.superclass.address
    
    sym_meta = sym.metaclass
    assert sym_meta.reference?
    assert_equal obj_meta.address, sym_meta.superclass.address
    
    ba = CPU::Global.bytearray
    hsh = CPU::Global.hash
    
    assert ba.reference?
    assert hsh.reference?
    assert_equal obj, ba.superclass
    assert_equal obj, hsh.superclass
    
  end
end


class TestCPUInstructions < Test::Unit::TestCase
  
  def setup
    @encoder = CPU::InstructionEncoder.new
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
end

class TestInstructionEncoder < Test::Unit::TestCase
  
  def setup
    @encoder = CPU::InstructionEncoder.new  
  end
  
  def decI(str, count=1)
    str.unpack("C" + ("I" * count))
  end
  
  def enc(*args)
    @encoder.encode *args
  end
  
  def assert_is_op(op, code)
    assert_equal CPU::InstructionEncoder::OpCodes[code], op, "#{code} is not #{op}"
  end
  
  def assert_is_op_only(op, bytes)
    assert_equal 1, bytes.size, "More than just #{op}."
    assert_is_op op, bytes[0]
  end
  
  def assert_op(op)
    assert_is_op_only op, enc(op)
  end
  
  def test_noop
    bytes = @encoder.encode :noop
    assert bytes[0] == 0
  end
  
  def test_push_nil
    assert_op :push_nil
  end
  
  def test_push_true
    assert_op :push_true
  end
  
  def test_push_false
    assert_op :push_false
  end
  
  def test_allocate
    bytes = @encoder.encode :allocate, 4
    assert_equal 6, bytes.size
    codes = decI bytes
    assert_is_op :push_int, codes[0]
    assert_equal 4, codes[1]
    assert_is_op :allocate, bytes[5]
  end
  
  def test_set_class
    bytes = @encoder.encode :set_class
    assert_equal 1, bytes.size
    codes = decI bytes
    assert_is_op :set_class, codes[0]
  end
  
  def test_find_field
    bytes = @encoder.encode :find_field, 1
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :find_field, codes[0]
    assert codes[1] == 1
  end
  
  def test_store_into
    bytes = @encoder.encode :store_into
    assert_equal 1, bytes.size
    assert_is_op :store_into, bytes[0]
  end
  
  def test_push_int
    bytes = @encoder.encode :push_int, 88
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :push_int, bytes[0]
    assert_equal 88, codes[1]
  end
  
  def test_store_field
    bytes = @encoder.encode :store_field
    assert_equal 1, bytes.size
    assert_is_op :store_field, bytes[0]
  end
  
  def test_store_field_at
    bytes = @encoder.encode :store_field_at, 354
    assert_equal 6, bytes.size
    codes = decI bytes
    assert_is_op :push_int, bytes[0]
    assert_equal 354, codes[1]
    assert_is_op :store_field, bytes[5]
  end

  def test_fetch_field
    assert_op :fetch_field
  end
  
  def test_send_primitive
    bytes = @encoder.encode :send_primitive, 88
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :send_primitive, bytes[0]
    assert_equal 88, codes[1]
  end
  
  def test_push_context
    assert_op :push_context
  end
  
  def assert_op_and_int(op, int)
    bytes = @encoder.encode op, int
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op op, bytes[0]
    assert_equal int, codes[1]
  end
  
  def test_push_literal
    bytes = @encoder.encode :push_literal, 2
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :push_literal, bytes[0]
    assert_equal 2, codes[1]
  end
  
  def test_push_self
    assert_op :push_self
  end
  
  def test_goto
    assert_op_and_int :goto, 6
  end
  
  def test_goto_if_false
    assert_op_and_int :goto_if_false, 6
  end
  
  def test_goto_if_true
    assert_op_and_int :goto_if_true, 6
  end
  
  def test_goto_if_defined
    assert_op_and_int :goto_if_defined, 8
  end
  
  def test_swap_stack
    assert_op :swap_stack
  end
  
  def test_set_local
    assert_op_and_int :set_local, 2
  end
  
  def test_push_local
    assert_op_and_int :push_local, 2
  end
  
  def test_push_exception
    assert_op :push_exception
  end
  
  def test_make_array
    assert_op_and_int :make_array, 20
  end
  
  def test_set_ivar
    assert_op_and_int :set_ivar, 12
  end
  
  def test_push_ivar
    assert_op_and_int :push_ivar, 12
  end
  
end
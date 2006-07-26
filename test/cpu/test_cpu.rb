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
    
    assert obj.access_metaclass.nil?
    assert cls.access_metaclass.nil?
    
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
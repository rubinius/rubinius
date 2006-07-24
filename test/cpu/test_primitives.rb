require 'cpu/primitives'
require 'test/unit'
require 'test/unit/show_code'

class TestCPUPrimitives < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @prim = CPU::Primitives.new(@cpu)
    @memory = @cpu.memory
  end
  
  def push(*args)
    args.each { |a| @cpu.stack_push a }
  end
  
  def mko(fel=4)
    push @cpu.new_blank(fel)
  end
  
  def push_int(i)
    push RObject.wrap(i).address
  end
  
  def push_obj(o)
    push o.address
  end
  
  def do_prim(prim)
    idx = CPU::Primitives::Primitives.index(prim)
    assert idx
    @prim.perform(idx)
  end
  
  def test_noop
    mko
    do_prim :noop
    assert @cpu.stack_empty?
  end
  
  def obj_top
    RObject.new(@cpu.stack_top)
  end
  
  def assert_stack_size(sz)
    assert_equal sz, @cpu.sp + 1, "Stack is not #{sz} objects big."
  end
  
  def test_add
    push_int 9
    push_int 10
    assert do_prim(:add)
    assert_equal 19, obj_top.to_int
    assert_stack_size 1
    
    push_int 9
    t = RObject.wrap(true)
    push_obj t
    assert !do_prim(:add)
  end
  
  def test_sp_preserved_on_error
    push_int 9
    push_obj RObject.wrap(true)
    cur = @cpu.sp
    assert !do_prim(:add)
    assert_equal cur, @cpu.sp
  end
  
  def test_equal
    push_int 9
    push_int 9
    assert do_prim(:equal)
    assert obj_top.true?
    
    push_int 9
    push_int 10
    assert do_prim(:equal)
    assert obj_top.false?
    
    push_int 9
    push_obj RObject.true
    assert !do_prim(:equal)
  end
  
  def test_compare
    push_int 9
    push_int 9
    assert do_prim(:compare)
    assert_equal 0, obj_top.to_int
    
    push_int 9
    push_int 10
    assert do_prim(:compare)
    assert_equal -1, obj_top.to_int
    
    push_int 10
    push_int 9
    assert do_prim(:compare)
    assert_equal 1, obj_top.to_int
    
    push_int 9
    push_obj RObject.true
    assert !do_prim(:compare)
  end
  
  def test_at
    obj = @memory.new_object CPU::NIL, 4
    obj2 = RObject.wrap(99)
    
    obj.put 1, obj2
    
    push_obj obj
    push_int 1
    assert do_prim(:at)
    assert_equal 99, obj_top.to_int
    
    push_obj obj
    push_int 5
    assert !do_prim(:at)
    
    push_int 5
    push_int 5
    assert !do_prim(:at)
    
  end
  
  def test_at_on_bytes
    obj = @memory.new_object CPU::NIL, 4
    obj.make_byte_storage
    push_obj obj
    push_int 1
    assert !do_prim(:at)
  end
  
  def new_obj(fel=4)
    obj = @memory.new_object CPU::NIL, fel
  end
  
  def test_put
    obj = @memory.new_object CPU::NIL, 4
    obj2 = RObject.wrap(99)
    
    push_obj obj
    push_obj obj2
    push_int 1
    
    assert do_prim(:put)
    ret = obj.at(1)
    
    assert_equal obj2, ret
    
    push_obj obj
    push_obj obj2
    push_int 5
    assert !do_prim(:put)
    
    push_int 5
    push_int 5
    push_int 1
    assert !do_prim(:put)
    
    obj.make_byte_storage
    push_obj obj
    push_obj obj2
    push_int 1
    assert !do_prim(:put)
  end
  
  def test_fields
    obj = new_obj 4
    
    push_obj obj
    assert do_prim(:fields)
    
    assert_equal 4, obj_top.to_int
    
    push_int 4
    assert !do_prim(:fields)
    
  end
  
  def test_allocate
    cls = new_obj 4
    cls.as :class
    cls.instance_fields = RObject.wrap(12)
    push_obj cls
    assert do_prim(:allocate)
    obj = obj_top()
    
    assert_stack_size 1
    assert_not_equal cls, obj
    
    assert_equal 12, obj.fields
    
    cls = new_obj 4
    push_obj cls
    assert !do_prim(:allocate)
    
    push_int 4
    assert !do_prim(:allocate)
  end
  
end
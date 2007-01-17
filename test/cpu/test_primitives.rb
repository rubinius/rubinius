require File.expand_path(File.dirname(__FILE__) + '/../test_helper')
require 'tempfile'
require 'bytecode/constructor'

class TestCPUPrimitives < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @con = Bytecode::Constructor.new(@cpu)
    @prim = CPU::Primitives.new(@cpu, @con)
    @memory = @cpu.memory
    @encoder = Bytecode::InstructionEncoder.new
    @nil = RObject.nil
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
  
  def do_prim(prim, mo=nil)
    idx = CPU::Primitives::Primitives.index(prim)
    assert idx
    @prim.perform(idx, mo)
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
    t = RObject.true
    push_obj t
    assert !do_prim(:add)
  end
  
  def test_sp_preserved_on_error
    push_int 9
    push_obj RObject.true
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

    push_int 10    
    push_int 9
    assert do_prim(:compare)
    assert_equal(-1, obj_top.to_int)

    push_int 9    
    push_int 10
    assert do_prim(:compare)
    assert_equal 1, obj_top.to_int

    push_obj RObject.true    
    push_int 9
    assert !do_prim(:compare)
  end
  
  def test_at
    obj = @memory.new_object @nil, 4
    obj2 = RObject.wrap(99)
    
    obj.put 1, obj2
    
    push_int 1
    push_obj obj
    assert do_prim(:at)
    assert_equal 99, obj_top.to_int
    
    push_int 5
    push_obj obj
    assert !do_prim(:at)
    
    push_int 5
    push_int 5
    assert !do_prim(:at)
    
  end
  
  def test_at_on_bytes
    obj = @memory.new_object @nil, 4
    obj.make_byte_storage
    push_int 1
    push_obj obj
    assert !do_prim(:at)
  end
  
  def new_obj(fel=4)
    obj = @memory.new_object @nil, fel
  end
  
  def test_put
    obj = @memory.new_object @nil, 4
    obj2 = RObject.wrap(99)

    push_obj obj2    
    push_int 1
    push_obj obj    
    assert do_prim(:put)
    ret = obj.at(1)
    
    assert_equal obj2, ret
    
    push_obj obj2
    push_int 5
    push_obj obj
    assert !do_prim(:put)
    
    push_int 5
    push_int 5
    push_int 1
    assert !do_prim(:put)
    
    obj.make_byte_storage
    push_obj obj2
    push_int 1
    push_obj obj
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
    cls = new_obj 8
    cls.as :class
    cls.instance_fields = RObject.wrap(12)
    push_obj cls
    assert do_prim(:allocate)
    obj = obj_top()
    
    assert_stack_size 1
    assert_not_equal cls, obj
    
    assert_equal 12, obj.fields
    
    cls = new_obj 8
    push_obj cls
    assert !do_prim(:allocate)
    
    push_int 4
    assert !do_prim(:allocate)
  end
  
  def test_allocate_count
    cls = new_obj 8
    cls.as :class
    push_obj RObject.wrap(12)
    push_obj cls
    assert do_prim(:allocate_count)
    obj = obj_top()
    
    assert_stack_size 1
    assert_not_equal cls, obj
    
    assert_equal 12, obj.fields
    
    cls = new_obj 8
    push_obj cls
    assert !do_prim(:allocate)
    
    push_int 4
    assert !do_prim(:allocate)
  end
  
  
  def test_create_block
    @cpu.bootstrap
    @cpu.push_object RObject.wrap(0)
    @cpu.push_object RObject.wrap(0)
    @cpu.push_object @cpu.active_context
    assert do_prim(:create_block)
    obj = obj_top()
    assert_stack_size 1
    assert_equal CPU::Global.blokenv, obj.rclass
  end
  
  def test_block_given
    @cpu.bootstrap
    assert do_prim(:block_given)
    assert obj_top.false?
    
    @cpu.block = Rubinius::Object.new
    assert do_prim(:block_given)
    assert obj_top.true?
  end
  
  def test_block_call
    @cpu.bootstrap
    bytes =  @encoder.encode :goto, 7
    bytes << @encoder.encode(:push_true)
    bytes << @encoder.encode(:soft_return)
    bytes << @encoder.encode(:push_false)
    
    bc = Rubinius::ByteArray.from_string bytes
    @cpu.active_context.bytecodes = bc
    @cpu.restore_context(@cpu.active_context)
    
    @cpu.push_object RObject.wrap(0)
    @cpu.push_object RObject.wrap(0)
    @cpu.push_object @cpu.active_context
    assert do_prim(:create_block)
    assert do_prim(:block_call)
    @cpu.run
    
    assert @cpu.pop_object.false?
    assert @cpu.pop_object.true?
  end
  
  def test_string_to_sexp
    @cpu.bootstrap
    str = Rubinius::String.new("true")
    @cpu.push_object str
    assert do_prim(:string_to_sexp)
    out = @cpu.pop_object
    assert CPU::Global.array, out.rclass
    out.as :array
    sym = out.get(0)
    assert sym.symbol?
    sym.as :symbol
    assert_equal ":true", sym.as_string
  end
  
  def test_load_file
    @cpu.bootstrap
    tmp = Tempfile.new("rb")
    tmp << "Blah = 9\n"
    tmp.close
    
    blah = Rubinius::String.new("Blah").to_sym    
    str = Rubinius::String.new(tmp.path)
    @cpu.push_object str
    assert do_prim(:load_file)
    @cpu.run
    out = @cpu.pop_object
    assert out.true?
    cnt = CPU::Global.object.constants
    cnt.as :hash
    out = cnt.find(blah)
    assert_equal 9, out.to_int
  end
  
  def test_io_write
    @cpu.bootstrap
    
    read, write = IO.pipe
    
    r_write = Rubinius::IO.new(write.fileno)
    
    str = Rubinius::String.new("hello")
    @cpu.push_object str
    @cpu.push_object r_write
    assert do_prim(:io_write)
    
    out = read.read(5)
    assert_equal "hello", out
  end
  
  def test_io_read
    @cpu.bootstrap
    
    read, write = IO.pipe
    
    r_read = Rubinius::IO.new(read.fileno)
    
    write << "hello"
    
    @cpu.push_object RObject.wrap(5)
    @cpu.push_object r_read
    assert do_prim(:io_read)
    str = @cpu.pop_object
    str.as :string
    
    assert_equal "hello", str.as_string
  end
  
  def test_io_read_short_read
    @cpu.bootstrap
    
    read, write = IO.pipe
    
    r_read = Rubinius::IO.new(read.fileno)
    
    write << "hel"
    
    @cpu.push_object RObject.wrap(5)
    @cpu.push_object r_read
    assert do_prim(:io_read)
    str = @cpu.pop_object
    str.as :string
    
    assert_equal "hel", str.as_string
  end
  
  def test_fixnum_to_s
    @cpu.bootstrap
    
    @cpu.push_object RObject.wrap(16)
    obj = RObject.wrap(99)
    @cpu.push_object obj
    assert do_prim(:fixnum_to_s)
    out = @cpu.pop_object
    out.as :string
    assert_equal "63", out.as_string
  end
  
  def test_logical_class
    @cpu.bootstrap
    
    obj = RObject.wrap(99)
    @cpu.push_object obj
    assert do_prim(:logical_class)
    out = @cpu.pop_object
    assert_equal CPU::Global.fixnum, out
  end
  
  def test_object_id
    @cpu.bootstrap
    obj = Rubinius::Tuple.new(1)
    @cpu.push_object obj
    assert do_prim(:object_id)
    out = @cpu.pop_object
    assert_equal obj.address, out.to_int
  end
  
  def test_hash_set
    @cpu.bootstrap
    obj = Rubinius::Hash.new
    key = RObject.wrap(32)
    @cpu.push_object RObject.wrap(88)
    @cpu.push_object key
    @cpu.push_object RObject.wrap(key.hash_int)
    @cpu.push_object obj
    assert do_prim(:hash_set)
    out = obj.find(key)
    assert_equal 88, out.to_int
  end
  
  def test_hash_get
    @cpu.bootstrap
    obj = Rubinius::Hash.new
    key = RObject.wrap(32)
    val = RObject.wrap(88)
    
    obj.set key, val
    
    @cpu.push_object key
    @cpu.push_object RObject.wrap(key.hash_int)
    @cpu.push_object obj
    assert do_prim(:hash_get)
    out = @cpu.pop_object
    assert_equal 88, out.to_int
  end
  
  def test_hash_object
    @cpu.bootstrap
    obj = Rubinius::Tuple.new(0)
    
    @cpu.push_object obj
    assert do_prim(:hash_object)
    hsh = @cpu.pop_object
    assert_equal obj.hash_int, hsh.to_int
  end
  
  def test_symbol_index
    @cpu.bootstrap
    sym = Rubinius::String.new("blah").to_sym
    
    @cpu.push_object sym
    assert do_prim(:symbol_index)
    idx = @cpu.pop_object
    assert_equal sym.symbol_index, idx.to_int
  end
  
  def test_dup_into
    @cpu.bootstrap
    obj = Rubinius::Hash.new
    h2 = Rubinius::Hash.new
    
    assert obj.values.address != h2.values.address
    @cpu.push_object obj
    @cpu.push_object h2
    assert do_prim(:dup_into)
    out = @cpu.pop_object
    assert_equal h2, out
    assert_equal obj.values, h2.values
  end
end

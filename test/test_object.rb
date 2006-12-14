require 'rubygems'
require 'test/unit' unless defined? $ZENTEST and $ZENTEST
require 'object'
require 'cpu/runtime'
require 'gc/heap'

class TestRObject < Test::Unit::TestCase
  def setup
    Rubinius.cpu = CPU.new
    @heap = Heap.new(256)
    @obj = RObject.setup(@heap, RObject.nil, 4)
    @one = RObject.wrap(1)
    @nil = RObject.nil
  end
  
  def teardown
    @heap.deallocate
  end
  
  # We override this and use Object#kind_of? instead of comparing off
  # obj.class like the normal assert_kind_of does because RObject#class
  # actually returns the class of the object the RObject controls.
  def assert_kind_of(klass, obj)
    assert obj.kind_of?(klass), "#{obj.inspect} was not a #{klass}"
  end
  
  def test_class_setup
    obj = RObject.setup(@heap, @nil, 4)
    assert_kind_of RObject, obj
  end

  def test_class_wrap
    obj = RObject.wrap(100)
    assert obj.fixnum?
  end
  
  def test_undef
    u = RObject.undef
    assert u.undef?
  end

  def test_address
    assert_equal @heap.address, @obj.address
  end

  def test_at
    @obj.put 0, @one
    out = @obj.at 0
    assert_kind_of RObject, out
    assert_equal @one, out
  end
  
  def test_class
    k = @obj.rclass
    assert_kind_of RObject, k
    assert k.nil?
    
    o2 = RObject.setup(@heap, @obj, 1)
    k = o2.rclass
    assert_equal k, @obj
  end

  def test_false_eh
    f = RObject.false
    assert f.false?
    assert !@one.false?
  end

  def test_fields
    assert_equal 4, @obj.fields
  end

  def test_fixnum_eh
    assert @one.fixnum?
    f = RObject.false
    
    assert !f.fixnum?
  end
  
  def test_to_int
    f = RObject.wrap(100)
    assert_equal 100, f.to_int
  end

  def test_flags
    assert_equal 0, @obj.flags
    @obj.flags = 6
    assert_equal 6, @obj.flags
  end
  
  def test_flags_overflow
    @obj.flags = 256
    assert_not_equal 256, @obj.flags
  end

  def test_flags2
    assert_equal 0, @obj.flags2
    @obj.flags2 = 6
    assert_equal 6, @obj.flags2    
  end
  
  def test_flags2_overflow
    @obj.flags2 = 256
    assert_not_equal 256, @obj.flags2
  end

  def test_flags2_equals
    @obj.flags2 = 88
    assert_equal 88, @obj.flags2
    
    @obj.flags2 = nil
    assert_equal 0, @obj.flags2
    
    assert_raises(NoMethodError) do
      @obj.flags2 = Object.new
    end
  end

  def test_flags_equals
    @obj.flags = 47
    assert_equal 47, @obj.flags
    
    @obj.flags = nil
    assert_equal 0, @obj.flags
    
    assert_raises(NoMethodError) do
      @obj.flags = Object.new
    end
  end

  def test_immediate_eh
    assert @one.immediate?
    
    assert RObject.true.immediate?
    assert RObject.false.immediate?
    assert RObject.nil.immediate?
  end

  def test_put
    assert_nothing_raised do
      @obj.put 0, @one
    end
    
    assert_raises(NoMethodError) do
      @obj.put 0, 0
    end
  end
  
  def test_bounds
    o = RObject.setup(@heap, @nil, 1)
    j = RObject.setup(@heap, @nil, 1)
    
    last = o.address + o.memory_size
    assert o.field_address(0) + 4 == j.address
    
  end
  
  def notest_old_bounds_code
    
    k = RObject.setup(@heap, nil, 1)
    m = RObject.setup(@heap, nil, 1)
    puts "o #{o.address}, #{o.fields}, #{o.memory_size}, #{o.address + o.memory_size}"
    puts "j #{j.address}, #{j.fields}, #{j.memory_size}, #{j.address + j.memory_size}"
    puts "k #{k.address}, #{k.fields}, #{k.memory_size}, #{k.address + k.memory_size}"
    puts "m #{m.address}, #{m.fields}, #{m.memory_size}, #{m.address + m.memory_size}"
    
    
    
    print "o => j "
    o.put 0, j
    print "j => k "
    j.put 0, k
    print "k => m "
    k.put 0, m
    
    p k.at(0)
    
    p m.memory_size
  end

  def test_reference_eh
    assert @obj.reference?
    assert !@one.reference?
  end

  def test_references
    o2 = RObject.setup(@heap, RObject.nil, 3)
    o3 = RObject.setup(@heap, RObject.nil, 3)
    @obj.put 0, o2
    @obj.put 1, o2
    @obj.put 2, o3
    
    refs = @obj.references
    
    assert_equal o2.address, refs[0].first.address
    assert_equal o2.address, refs[1].first.address
    assert_equal o3.address, refs[2].first.address
  end
  
  def test_references_none
    @obj.put 0, @one
    refs = @obj.references
    assert refs.empty?
  end

  def test_true_eh
    t = RObject.true
    assert t.true?
  end
  
  def test_fields_default_to_false
    t = RObject.setup @heap, RObject.nil, 4
    
    assert_equal @nil, t.at(0)
    assert_equal @nil, t.at(1)
    assert_equal @nil, t.at(2)
    assert_equal @nil, t.at(3)
    
  end
  
  def test_memory_size
    sz = @obj.memory_size
    calc = RObject::HeaderSize + (@obj.fields * 4)
    assert_equal calc, sz
  end
  
  def test_negative_fixnum
    obj = RObject.wrap(-1)
    assert obj.fixnum_neg?
    assert_equal -1, obj.to_int
  end
  
  def test_symbol
    obj = RObject.symbol(1)
    assert obj.symbol?
    assert_equal 1, obj.symbol_index
  end
  
  def test_stores_bytes_eh
    t = RObject.true
    assert !t.stores_bytes?
    
    t = RObject.wrap(1)
    assert !t.stores_bytes?
    
    assert !@obj.stores_bytes?
    
    @obj.make_byte_storage
    
    assert @obj.stores_bytes?
  end
  
  def test_s_setup_bytes
    obj = RObject.setup_bytes @heap, RObject.nil, 4
    assert obj.stores_bytes?
  end
  
  def test_store_and_fetch_byte
    obj = RObject.setup_bytes @heap, RObject.nil, 4
    obj.store_byte 0, 99
    out = obj.fetch_byte 0
    assert_equal 99, out
  end
  
  def test_copy_fields
    obj = RObject.setup(@heap, RObject.nil, 3)
    obj2 = RObject.setup(@heap, RObject.nil, 2)
    
    obj.put 0, RObject.wrap(99)
    obj.put 1, RObject.wrap(3333)
    obj.put 2, RObject.wrap(9812)
    
    obj.copy_fields obj2, 2
    
    assert_equal 99, obj2.at(0).to_int
    assert_equal 3333, obj2.at(1).to_int
  end
  
  def test_copy_fields_from
    obj = RObject.setup(@heap, RObject.nil, 3)
    obj2 = RObject.setup(@heap, RObject.nil, 2)
    
    obj.put 0, RObject.wrap(99)
    obj.put 1, RObject.wrap(3333)
    obj.put 2, RObject.wrap(9812)
    
    obj.copy_fields_from obj2, 1, 1
    
    assert_equal 3333, obj2.at(0).to_int
  end
  
end

require 'test/unit' unless defined? $ZENTEST and $ZENTEST
require 'gc/heap'

class TestHeap < Test::Unit::TestCase
  def setup
    @heap = Heap.new(200)
  end
  
  def teardown
    @heap.deallocate
  end
  
  def test_address
    assert_kind_of Fixnum, @heap.address
  end

  def test_allocate
    mem = @heap.allocate(10)
    assert_kind_of Fixnum, mem
    assert_equal 10, @heap.current
  end
  
  def test_allocate_not_enough
    mem = @heap.allocate(256)
    assert mem.nil?
  end
  
  def test_allocated_eh
    assert @heap.allocated?
    @heap.deallocate
    assert !@heap.allocated?
  end

  def test_current
    assert_equal 0, @heap.current
    mem = @heap.allocate 10
    assert_equal @heap.address, mem
    assert_equal 10, @heap.current
    
    mem = @heap.allocate 20
    assert_equal 30, @heap.current
    assert_equal mem, @heap.address + 10
  end

  def test_enough_space_eh
    assert @heap.enough_space?(10)
    assert !@heap.enough_space?(201)
  end

  def test_size
    assert_equal 200, @heap.size
  end
  
  def test_deallocate
    @heap.deallocate
    assert_equal 0, @heap.address
  end
  
  def test_contains_p
    mem = @heap.allocate(20)
    assert @heap.contains?(mem)
    assert !@heap.contains?(90)
    lst = @heap.address + @heap.size
    assert !@heap.contains?(lst)
  end
  
  def test_copy_object
    o1 = RObject.setup @heap, nil, 2
    o2 = RObject.setup @heap, nil, 1
    
    h = Heap.new(200)
    
    o1.put 0, RObject.wrap(10)
    o1.put 1, o2
    
    o3 = h.copy_object o1
    assert h.contains?(o3.address)
    assert_not_equal o1.address, o3.address
    
    assert_equal o1.at(1), o2
  end
  
  def test_each_object
    lo = []
    lo << RObject.setup(@heap, nil, 2)
    lo << RObject.setup(@heap, nil, 3)
    lo << RObject.setup(@heap, nil, 10)
    
    fo = []
    @heap.each_object do |obj|
      fo << obj
    end
    
    assert_equal lo, fo
  end
  
  def test_reset_bang
    addr = @heap.allocate(10)
    @heap.reset!
    a2 = @heap.allocate(10)
    assert_equal addr, a2
  end
end

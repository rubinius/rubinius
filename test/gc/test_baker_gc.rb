require File.expand_path(File.dirname(__FILE__) + '/../test_helper')
require 'gc/baker_gc'

class TestBakerGC < Test::Unit::TestCase
  
  def setup
    @bk = BakerGC.new(200)
    @nil = RObject.nil
    
    @o1 = RObject.setup @bk, @nil, 4
    @o2 = RObject.setup @bk, @nil, 2
    @o3 = RObject.setup @bk, @nil, 1
    @o4 = RObject.setup @bk, @nil, 1
    
    @local1 = @o1.dup
    @local2 = @o2.dup
    @local3 = @o3.dup
    @local4 = @o4.dup
    
    @bk.on_stack @o1, @o2, @o3, @o4
  end
  
  def teardown
    @bk.destroy!
  end
  
  def test_collect_from
    @o1.put 0, @o2
    
    local1 = @o1.dup
    local2 = @o2.dup
    @bk.collect_from @o1
    
    assert_not_equal local1, @o1
    assert_not_equal local2, @o2
    
    assert_equal @o1.at(0), @o2
  end
  
  def test_collect_cycle
    local1 = @o1.dup
    @o1.put 0, @o1
    @bk.collect_from @o1
    
    assert @bk.forwarded?(local1)
    assert_equal @bk.forwarding_object(local1), @o1 
    assert_equal @o1.at(0), @o1
  end
  
  def test_collect_cycle2
    @o3.put 0, @o1
    @o1.put 0, @o2
    @o2.put 0, @o1
    
    @bk.collect_from @o3
    assert_not_equal @o1, @local1
    assert_not_equal @o2, @local2
    assert_not_equal @o3, @local3
  end
  
  def test_collect_depth4
    @o1.put 0, @o2
    @o2.put 0, @o3
    @o3.put 0, @o4
    
    @bk.collect_from @o1
    assert_not_equal @o1, @local1
    assert_not_equal @o1.at(0), @local2
    assert_not_equal @o2.at(0), @local3
    assert_not_equal @o3.at(0), @local4
    assert_equal @o2, @o1.at(0)
    assert_equal @o3, @o2.at(0)
    assert_equal @o4, @o3.at(0)
  end
  
  def test_collect_only_mine
    heap = Heap.new(200)
    ext = RObject.setup heap, @nil, 1
    @bk.on_stack ext
    
    local_ext = ext.dup
    
    @o1.put 0, @o2
    @o1.put 1, ext
    
    @bk.collect_from @o1
    
    assert_equal local_ext, ext
    heap.deallocate
  end
  
  def test_collect_from_external_root
    heap = Heap.new(200)
    ext = RObject.setup heap, @nil, 1
    @bk.on_stack ext
    
    local_ext = ext.dup
    
    ext.put 0, @o1
    
    @bk.collect_from ext
    
    assert_equal local_ext, ext
    assert_not_equal @local1, @o1
    heap.deallocate
  end
  
  def test_collect
    @o1.put 0, @o2
    @o3.put 0, @o2
    
    @bk.collect [@o1, @o3]
    
    assert_not_equal @local1, @o1
    assert_not_equal @local2, @o2
    assert_not_equal @local3, @o3
    assert_equal @local4, @o4
  end
  
  def test_collect_remember_set
    h = Heap.new(200)
    ext = RObject.setup h, @nil, 3
    ext.put 0, @o1
    @bk.remember_set << ext
    @bk.collect []
    assert_not_equal @o1, @local1
  end
  
  def test_swap
    class << @bk
      attr_accessor :current, :next, :space_a, :space_b, :stack
    end
    
    cur = @bk.current
    nxt = @bk.next

    last_o1 = @o1.dup
    6.times do |i|
      @bk.collect [@o1]
      assert_not_equal(last_o1, @o1, "Address should have changed on #{i}th iteration")
      last_o1 = @o1.dup
    end
        
    @bk.swap!
    
    assert_equal cur, @bk.next
    assert_equal nxt, @bk.current
    
    assert_equal 0, @bk.next.current - @bk.next.address
  end
end

require 'gc/mark_sweepgc'
require 'test/unit'

class TestMarkSweepGC < Test::Unit::TestCase
  def setup
    @ms = MarkSweepGC.new(200)
    class << @ms
      attr_accessor :allocations, :start
    end
    
    
    @o1 = RObject.setup @ms, nil, 4
    @o2 = RObject.setup @ms, nil, 2
    @o3 = RObject.setup @ms, nil, 1
    @o4 = RObject.setup @ms, nil, 1
    
    @local1 = @o1.dup
    @local2 = @o2.dup
    @local3 = @o3.dup
    @local4 = @o4.dup
  end
  
  def teardown
    @ms.destroy!
  end
  
  def test_mark
    @o1.put 0, @o2
    @ms.collect [@o1]
    
    assert @ms.object_marked?(@o1)
    assert @ms.object_marked?(@o2)
    assert !@ms.object_marked?(@o3)
    assert !@ms.object_marked?(@o4)
  end
  
  def test_mark_object
    old = @o1.flags
    @ms.mark_object @o1
    assert_not_equal old, @o1.flags
    assert @ms.object_marked?(@o1)    
  end
  
  def test_unmark_object
    old = @o1.flags
    @ms.mark_object @o1
    @ms.unmark_object @o1
    assert_equal old, @o1.flags
  end
  
  def test_regions_in_use
    class << @ms
      attr_accessor :allocations, :start
    end
    @ms.allocations << [@ms.start + 325, 80]
    @ms.allocations << [@ms.start + 250, 50]
    it = @o1.memory_size + @o2.memory_size + @o3.memory_size + @o4.memory_size
    # p @ms.allocations
    s = @ms.start
    in_use = @ms.regions_in_use
    tv = [[s, it], [s+200, 50], [s+300, 25]]
    assert_equal tv, in_use
  end
  
  def test_each_object
    out = []
    @ms.each_object do |obj|
      out << obj
    end
    
    assert_equal @o1, out[0]
    assert_equal @o2, out[1]
    assert_equal @o3, out[2]
    assert_equal @o4, out[3]    
  end
  
  def test_clear_marks
    @o1.put 0, @o2
    @ms.collect [@o1]
    
    @ms.clear_marks!
    
    assert !@ms.object_marked?(@o1)
  end
  
  def test_sweep
    
    @o1.put 0, @o2
    @ms.collect [@o1]
    
    # TODO main problem with this test is that it also expects
    # a certain behavior of how @ms.allocations is populated.
    # Thats ok, but this test might fail if that behavior changes.
    
    ending = @ms.allocations.first
    @ms.sweep
    current = @ms.allocations
    assert_equal [@o4.address, 16], current[0]
    assert_equal [@o3.address, 16], current[1]
    assert_equal ending, current[2]
  end
  
  def test_reuse
    @o1.put 0, @o2
    @ms.collect [@o1]
    
    @ms.sweep
    @ms.reorder!
    n = RObject.setup @ms, nil, 1
    curr =  @ms.allocations
    assert_equal 1, curr.size
    assert_equal @o4.address, curr.first.first
    assert_equal @o3.address, n.address 
  end
  
  def test_complete_cycle
    @ms.complete_cycle []
    curr = @ms.allocations.first
    assert_equal @ms.start, curr.first
    assert_equal 200, curr.last
  end
end

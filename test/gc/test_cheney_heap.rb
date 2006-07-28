require 'cheney_heap'
require 'test/unit'

class TestCheneyHeap < Test::Unit::TestCase
  def setup
    @ch = CheneyHeap.new(1000)
  end
  
  def test_scan
    assert_kind_of Fixnum, @ch.scan
  end
  
  def test_next
    assert_kind_of Fixnum, @ch.next
  end
  
  def test_fully_scanned_eh
    assert @ch.fully_scanned?
    @ch.next = @ch.scan + 1
    assert !@ch.fully_scanned?
  end
  
  def test_allocate
    addr = @ch.allocate(20)
    assert_equal @ch.scan + 20, @ch.next
  end
  
  def test_unscanned_objects
    o1 = RObject.setup @ch, nil, 3
    o2 = RObject.setup @ch, nil, 4
    ary = []
    @ch.unscanned_objects { |o| ary << o }
    assert_equal [o1, o2], ary
    o3 = RObject.setup @ch, nil, 3
    @ch.unscanned_objects do |o|
      assert_equal o3, o
    end
    ary = []
    @ch.unscanned_objects { |o| ary << o }
    assert_equal [], ary
  end
  
  def test_alloc_during_unscanned
    o1 = RObject.setup @ch, nil, 3
    o2 = nil
    ary = []
    @ch.unscanned_objects do |o|
      ary << o
      o2 = RObject.setup @ch, nil, 2 unless o2
    end
    assert o2
    assert_equal [o1, o2], ary
  end
end
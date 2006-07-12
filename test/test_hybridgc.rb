require 'hybridgc'
require 'test/unit'

class TestHybridGC < Test::Unit::TestCase
  def setup
    @hy = HybridGC.new(100,500)
    class << @hy
      attr_accessor :eden, :mature
    end
    class << @hy.eden
      def start
        @current.address
      end
    end
    
    class << @hy.mature
      attr_reader :start
    end
  end
  
  def util_o1
    @o1 = RObject.setup @hy, nil, 4
    @hy.on_stack @o1
  end
  
  def test_allocate
    addr = @hy.allocate(10)
    assert_equal @hy.eden.start, addr
    a2 = @hy.allocate(10)
    assert_equal @hy.eden.start + 10, a2
  end
  
  def test_clean_eden
    util_o1
    o2 = RObject.setup @hy, nil, 4
    @hy.clean_eden [@o1]
    assert_equal @hy.eden.start, @o1.address
  end
  
  def test_promote
    util_o1
    old1 = @o1.dup
    @hy.promote @o1, @hy.mature
    assert_equal @hy.mature.start, @o1.address
  end
  
  def test_promotion_round
    util_o1
    old1 = @o1.dup
    @hy.promote_between(@hy.eden, @hy.mature)
    assert_equal old1, @o1
    assert @hy.object_marked?(@o1)
    
    o2 = RObject.setup @hy, nil, 4
    o2.put 0, @o1
    
    @hy.promote_between(@hy.eden, @hy.mature)
    assert_not_equal old1, @o1
    assert !@hy.object_marked?(@o1)
    assert @hy.mature.contains?(@o1)
    assert_equal @o1, o2.at(0)
  end
  
  def test_compact
    util_o1
    
    assert @hy.immature?(@o1)
    
    # Compact 2 times first...
    2.downto(0) do |i|
      # puts "o1: #{@o1.address}"
      assert_equal @hy.compacts_til_survivor, i
      assert @hy.immature?(@o1)
      @hy.compact [@o1]
    end
        
    # Check that the object gets marked..
    assert @hy.object_marked?(@o1)
    
    # Then another 2 times..
    2.downto(0) do |i|
      # puts "o1: #{@o1.address}"
      assert_equal @hy.compacts_til_survivor, i
      assert @hy.immature?(@o1)
      @hy.compact [@o1]
      # puts "o1 after: #{@o1.address}"
    end
    
    assert !@hy.object_marked?(@o1)
    
    # And magically it's been moved to the survivor space!
    assert @hy.survivor?(@o1)
    
    # Now, lets compact enough to get it into the mature space!
    
    5.downto(0) do |i|
      assert_equal @hy.compacts_til_mature, i
      assert @hy.survivor?(@o1)
      @hy.compact [@o1]
    end
    
    assert @hy.object_marked?(@o1)
    
    # Again 5 times..
    5.downto(0) do |i|
      assert_equal @hy.compacts_til_mature, i
      assert @hy.survivor?(@o1)
      @hy.compact [@o1]
    end
    
    # Zonks! Mature space object!
    assert @hy.mature?(@o1)
  end
  
  def test_remember_set
    util_o1
    
    6.times { @hy.compact [@o1] }
    assert @hy.survivor?(@o1)
    
    o2 = RObject.setup @hy, nil, 3
    o3 = RObject.setup @hy, nil, 3
    @hy.on_stack o2, o3
    
    assert @hy.immature?(o2)
    
    @o1.put 0, o2
    @hy.update_remember_sets @o1, o2
    # Compact without listing @o1 so that if o2 is going to be marked
    # properly, it should come from the remember_set of @eden.
    @hy.compact [o3]
    assert @hy.immature?(o2)
    assert @hy.immature?(o3)
    
    11.times { |i| p i; @hy.compact [@o1, o3] }
    assert @hy.mature?(@o1)
    assert @hy.survivor?(o2)
    assert @hy.survivor?(o3)

    # Collect enough for survivor to get swept.
    3.times { @hy.compact [] }

    assert @hy.survivor?(o2)    
  end
end
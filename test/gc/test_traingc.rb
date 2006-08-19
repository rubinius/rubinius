require 'gc/traingc'
require 'test/unit'

class TestTrainGC < Test::Unit::TestCase
  def setup
    @num_trains = 2
    @train = TrainGC.new(@num_trains,1)
    RObject.use_write_barrier @train
    @tracked = []
  end
  
  def show_tracked
    @tracked.each do |obj|
      puts "#{obj.tag}: #{obj.address}"
    end
  end
  
  def teardown
    RObject.remove_write_barrier
    @train.destroy!
  end
  
  def test_allocate
    addr = @train.allocate(15)
    assert_kind_of Fixnum, addr
  end
  
  def test_trains
    ary = @train.trains
    assert_kind_of Array, ary
    assert_equal @num_trains, ary.size
    ary.each { |t| assert_kind_of TrainGC::Train, t }
  end
  
  def test_calculate_index
    addr = 2275231319
    out = @train.calculate_index(addr)
    assert_equal 157, out
  end
  
  def thobj(where, tag)
    # Setup the default object size so that only 3 objects
    # can fit in each car. (each car is 64k bytes big.)
    obj_fields = 5000
    r = RObject.setup where, nil, 5000
    r.tag = tag
    @tracked << r
    @train.on_stack r
    return r
  end
  
  def show_train
    @train.trains.each do |train|
      next unless train
      print "#{train.index}: "
      puts train.cars.map { |c| c.inspect }.join(", ")
    end
  end
  
  def assert_contains(cont, *objs)
    objs.each do |o| 
      assert cont.contains?(o.address), "#{o.inspect} was not in #{cont.inspect}"
    end
  end
  
  def assert_doesnt_contain(cont, *objs)
    objs.each do |o| 
      assert !cont.contains?(o.address), "#{o.inspect} was in #{cont.inspect}"
    end
  end
  
  def test_thesis_1
    train1 = @train.trains[0]
    train2 = @train.trains[1]
    
    train1.add_car
    train1.add_car
    
    r = thobj train1.cars[0], :r
    a = thobj train1.cars[0], :a
    c = thobj train1.cars[0], :c
    co = train1.cars[0]
    cidx = @train.find_address_info co

    # puts "r info: #{r.address}, idx: #{@train.find_address_info(r)}"
    # puts "a info: #{a.address}, idx: #{@train.find_address_info(a)}"
    # puts "c info: #{c.address}, idx: #{@train.find_address_info(c)}"

    assert_equal cidx, @train.find_address_info(r)
    assert_equal cidx, @train.find_address_info(a)
    assert_equal cidx, @train.find_address_info(c)
    assert_contains train1.cars[0], r, a, c    
    
    s = thobj train1.cars[1], :s
    d = thobj train1.cars[1], :d
    e = thobj train1.cars[1], :e
    assert_contains train1.cars[1], s, d, e
    
    t = thobj train1.cars[2], :t
    f = thobj train1.cars[2], :f
    assert_contains train1.cars[2], t, f
    
    b = thobj train2.cars[0], :b
    assert_contains train2.cars[0], b
    
    # r is only root object.
    roots = [r]
    
    # puts "c info: #{c.address}, idx: #{@train.calculate_index(c.address)}"
    # Setup RST
    r.put 0, s
    s.put 0, t
    
    # Setup AB
    a.put 0, b
    b.put 0, a
    
    # Setup CDEF
    c.put 0, d
    d.put 0, e
    e.put 0, f
    f.put 0, c
    
    # show_tracked
    
    # Now, assert that the remember_sets have been created properly
    # before we start.
    assert_equal [b], train1.remember_set
    assert_equal [b, f], train1.cars[0].remember_set
    assert_equal [],  train1.cars[1].remember_set
    assert_equal [],  train1.cars[2].remember_set
    assert_equal [],  train2.cars[0].remember_set
    assert_equal [],  train2.remember_set
    
    # show_train
    
    @train.invoke roots
    # puts "========="
    # show_tracked
    # show_train
    
    c12 = train1.cars[1]
    c13 = train1.cars[2]
    c21 = train2.cars[0]
    
    assert_contains c12, s, d, e
    assert_contains c13, t, f, c
    assert_contains c21, r, b, a
    
    # assert c12.contains?(s.address)
    # assert c12.contains?(d.address)
    # assert c12.contains?(e.address)
    
    # assert c13.contains?(t.address)
    # assert c13.contains?(f.address)
    # assert c13.contains?(c.address)
    
    # puts "r: #{r.address}, #{c21.inspect}"
    # assert c21.contains?(r.address)
    # assert c21.contains?(b.address)
    # assert c21.contains?(a.address)
    
    assert_equal [r], train1.remember_set
    assert_equal nil, train1.cars[0]
    assert_equal [r, c], train1.cars[1].remember_set
    assert_equal [],  train1.cars[2].remember_set
    assert_equal [],  train2.remember_set
    assert_equal [],  train2.cars[0].remember_set
        
    @train.invoke roots
    
    # show_tracked
    # show_train
    
    c14 = train1.cars[3]
    c22 = train2.cars[1]
    
    assert_contains c13, t, f, c
    assert_contains c14, d, e
    assert_contains c21, b, a, r
    assert_contains c22, s
    
    assert_equal [s], train1.remember_set
    assert_equal nil, train1.cars[1]
    assert_equal [s, e], train1.cars[2].remember_set
    assert_equal [], train1.cars[3].remember_set
    assert_equal [], train2.cars[0].remember_set
    assert_equal [], train2.cars[1].remember_set
    
    @train.invoke roots
    
    c15 = train1.cars[4]
    
    assert_contains c14, d, e, f
    assert_contains c15, c
    assert_contains c21, b, a, r
    assert_contains c22, s, t
    
    assert_equal [], train1.remember_set
    assert_equal [c], c14.remember_set
    assert_equal [], c15.remember_set
    assert_equal [], c21.remember_set
    assert_equal [], c22.remember_set
    
    @train.invoke roots
    
    assert @train.trains[0].nil?
    assert_contains c21, b, a, r
    assert_contains c22, s, t
    
    @train.invoke roots
    
    assert_contains c22, r, s, t
    assert_doesnt_contain c22, b, a
    assert @train.trains[1].cars[0].nil?
    
    # show_tracked
    # show_train
  end
end

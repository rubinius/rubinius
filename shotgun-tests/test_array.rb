require File.dirname(__FILE__) + '/helper'

class TestArray < RubiniusTestCase
  
  def test_array_each
    out = rp <<-CODE
    ary = [1,2,3]
    ary.each do |i|
      p i
    end
    CODE
    
    assert_equal ['1','2','3'], out
  end

  def test_array_partition
    out = rp <<-CODE
      a = [0,1,2,3,4,5]
      l, r = a.partition {|e| [0,2,4].include?(e)}
      puts "even: #{l.inspect}"
      puts "odd: #{r.inspect}"
    CODE
    assert_equal ['even: [0, 2, 4]', 'odd: [1, 3, 5]'], out
  end

  def test_array_uniq
    out = rp <<-CODE
      a = [1,1,2,2,2,3,4,5,5]
      puts a.uniq.inspect
    CODE
    assert_equal ['[1, 2, 3, 4, 5]'], out
  end

  def test_array_uniq!
    out = rp <<-CODE
      a = [ "a", "a", "b", "b", "c" ]
      puts a.uniq!.inspect   #=> ["a", "b", "c"]
      b = [ "a", "b", "c" ]
      puts b.uniq!.inspect   #=> nil
    CODE
    assert_equal ['["a", "b", "c"]', 'nil'], out
  end
end

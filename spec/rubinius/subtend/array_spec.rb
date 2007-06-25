require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

extension :rubinius do
  compile_extension('subtend_array')
  require File.dirname(__FILE__) + '/ext/subtend_array'
  
  context "SubtendArray" do
    setup do
      @s = SubtendArray.new
    end
  
    specify "rb_ary_new should return an empty array" do
      @s.new_array.should == []
    end
  
    specify "rb_ary_push should add an element to an array" do
      @s.rb_ary_push([], 4).should == [4]
    end
  
    specify "rb_ary_push2 should add elements to an array" do
      o = Object.new
      @s.rb_ary_push2([], 4, o).should == [4,o]
    end

    specify "rb_ary_entry should return nil when called with an empty array" do
      @s.rb_ary_entry([], 0).should == nil
    end

    specify "rb_ary_entry should wrap around the offset if it's negative" do
      @s.rb_ary_entry([1, 2, 3], -1).should == 3
      @s.rb_ary_entry([1, 2, 3], -2).should == 2
    end

    specify "rb_ary_entry should return nil if the index is out of range" do
      @s.rb_ary_entry([1, 2, 3], 3).should == nil
      @s.rb_ary_entry([1, 2, 3], -10).should == nil
    end

    specify "rb_ary_clear should remove all elements from the array" do
      @s.rb_ary_clear([]).should == []
      @s.rb_ary_clear([1, 2, 3]).should == []
    end

    specify "rb_ary_dup should duplicate the array" do
      @s.rb_ary_dup([]).should == []

      a = [1, 2, 3]
      b = @s.rb_ary_dup(a)

      b.should == a
      b.should_not equal?(a)
    end

    specify "rb_ary_unshift should prepend the element to the array" do
      a = [1, 2, 3]
      @s.rb_ary_unshift(a, "a").should == ["a", 1, 2, 3]
      a.should == ['a', 1, 2, 3]
    end

    specify "rb_ary_shift should remove and return the first element" do
      a = [5, 1, 1, 5, 4]
      @s.rb_ary_shift(a).should == 5
      a.should == [1, 1, 5, 4]
    end

    specify "rb_ary_shift should return nil when the array is empty" do
      @s.rb_ary_shift([]).should == nil
    end

    specify "rb_ary_store should overwrite the element at the given position" do
      a = [1, 2, 3]

      @s.rb_ary_store(a, 1, 5)

      a.should == [1, 5, 3]
    end

    specify "rb_ary_store should wrap around the offset if it's negative" do
      a = [1, 2, 3]

      @s.rb_ary_store(a, -1, 5)

      a.should == [1, 2, 5]
    end

    specify "rb_ary_store should raise IndexError if the offset is still negative after wrap around " do
      a = [1, 2, 3]

      should_raise IndexError do
        @s.rb_ary_store(a, -10, 5)
      end
    end

    specify "rb_ary_store should enlarge the array if necessary" do
      a = []

      @s.rb_ary_store(a, 2, 7)

      a.should == [nil, nil, 7]
    end
  end
end

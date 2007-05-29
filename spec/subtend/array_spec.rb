require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/array/subtend_array'

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
end

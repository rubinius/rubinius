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
end

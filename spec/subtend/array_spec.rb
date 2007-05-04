require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/array/subtend_array'

context "SubtendArray" do
  setup do
    @s = SubtendArray.new
  end
  
  specify "rb_ary_new should return an empty array" do
    @s.rb_ary_new.should == []
  end
end
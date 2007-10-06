require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#+" do
  it "concatenates two arrays" do
    ([ 1, 2, 3 ] + [ 3, 4, 5 ]).should == [1, 2, 3, 3, 4, 5]
    ([ 1, 2, 3 ] + []).should == [1, 2, 3]
    ([] + [ 1, 2, 3 ]).should == [1, 2, 3]
    ([] + []).should == []
  end

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() ["x", "y"] end
    ([1, 2, 3] + obj).should == [1, 2, 3] + obj.to_ary
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [:x])
    ([1, 2, 3] + obj).should == [1, 2, 3, :x]
  end

  it "does return subclass instances with Array subclasses" do
    (MyArray[1, 2, 3] + []).class.should == Array
    (MyArray[1, 2, 3] + MyArray[]).class.should == Array
    ([1, 2, 3] + MyArray[]).class.should == Array
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6] + ToAryArray[1, 2]).should == [5, 6, 1, 2]
  end
end

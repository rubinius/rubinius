require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#|" do
  it "returns an array of elements that appear in either array (union)" do
    ([] | []).should == []
    ([1, 2] | []).should == [1, 2]
    ([] | [1, 2]).should == [1, 2]
    ([ 1, 2, 3, 4 ] | [ 3, 4, 5 ]).should == [1, 2, 3, 4, 5]
  end
  
  it "creates an array with no duplicates" do
    ([ 1, 2, 3, 1, 4, 5 ] | [ 1, 3, 4, 5, 3, 6 ]).should == [1, 2, 3, 4, 5, 6]
  end
  
  it "creates an array with elements in order they are first encountered" do
    ([ 1, 2, 3, 1 ] | [ 1, 3, 4, 5 ]).should == [1, 2, 3, 4, 5]
  end

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ([0] | obj).should == ([0] | obj.to_ary)
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [1, 2, 3])
    ([0] | obj).should == [0, 1, 2, 3]
  end

  # MRI doesn't actually call eql?() however. So you can't reimplement it.
  it "acts as if using eql?" do
    ([5.0, 4.0] | [5, 4]).should == [5.0, 4.0, 5, 4]
    str = "x"
    ([str] | [str.dup]).should == [str]
  end
  
  it "does not return subclass instances for Array subclasses" do
    (MyArray[1, 2, 3] | []).class.should == Array
    (MyArray[1, 2, 3] | MyArray[1, 2, 3]).class.should == Array
    ([] | MyArray[1, 2, 3]).class.should == Array
  end
  
  it "does not call to_ary on array subclasses" do
    ([1, 2] | ToAryArray[5, 6]).should == [1, 2, 5, 6]
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#Array" do
  it "first tries to call #to_ary on the given argument" do
    (obj = Object.new).should_receive(:to_ary, :returning => [1, 2, 3])
    obj.should_not_receive(:to_a)
    
    Array(obj).should == [1, 2, 3]
  end
  
  it "tries to call #to_a on the given argument if #to_ary is not provided" do
    (obj = Object.new).should_receive(:to_a, :returning => [4, 5, 6])
    Array(obj).should == [4, 5, 6]
  end
  
  it "returns an array with the given argument if neither #to_ary nor #to_a are provided" do
    obj = Object.new
    Array(obj).should == [obj]
  end
  
  it "returns an empty array if the given argument is nil" do
    Array(nil).should == []
  end
  
  it "raises a TypeError if #to_ary / #to_a do not return an array" do
    (obj = Object.new).should_receive(:to_a, :returning => "ha!")
    lambda { Array(obj) }.should raise_error(TypeError)

    obj.should_receive(:to_ary, :returning => "ha!")
    lambda { Array(obj) }.should raise_error(TypeError)
  end
end

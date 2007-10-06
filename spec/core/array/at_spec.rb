require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#at" do
  it "returns the element at index" do
    a = [1, 2, 3, 4, 5, 6]
    a.at(0).should  == 1
    a.at(-2).should == 5
    a.at(10).should == nil
  end

  it "calls to_int on its argument" do
    a = ["a", "b", "c"]
    a.at(0.5).should == "a"
    
    obj = Object.new
    obj.should_receive(:to_int, :returning => 2)
    a.at(obj).should == "c"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    a.at(obj).should == "c"
  end
end

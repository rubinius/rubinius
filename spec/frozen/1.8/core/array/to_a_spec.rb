require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#to_a" do
  it "returns self" do
    a = [1, 2, 3]
    a.to_a.should == [1, 2, 3]
    a.should equal(a.to_a) 
  end
  
  it "does not return subclass instance on Array subclasses" do
    e = ArraySpecs::MyArray.new
    e << 1
    e.to_a.class.should == Array
    e.to_a.should == [1]
  end
end

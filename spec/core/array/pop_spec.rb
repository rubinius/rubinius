require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#pop" do
  it "removes and returns the last element of the array" do
    a = ["a", 1, nil, true]
    
    a.pop.should == true
    a.should == ["a", 1, nil]

    a.pop.should == nil
    a.should == ["a", 1]

    a.pop.should == 1
    a.should == ["a"]

    a.pop.should == "a"
    a.should == []
  end
  
  it "returns nil if there are no more elements" do
    [].pop.should == nil
  end

  compliant :mri do
    it "raises TypeError on a frozen array" do
      should_raise(TypeError) { @frozen_array.pop }
    end
  end
end

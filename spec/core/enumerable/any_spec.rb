require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#any?" do
  it "any? with no block should return true if any element is not false or nil" do
    Numerous.new.any?.should == true 
    Numerous.new(1, nil, 2).any?.should == true 
    Numerous.new(false).any?.should == false     
  end
  
  it "any? should return true if the block ever returns other than false or nil" do
    Numerous.new.any? { |i| i == 5 }.should == true
  end
  
  it "any? should return false if the block never returns other than false or nil" do
    Numerous.new.any? { false }.should == false
    Numerous.new.any? { nil }.should == false
  end
  
  it "any? with no block should return true if any element is not false or nil (Legacy form rubycon)" do
    EachDefiner.new('a','b','c').any?.should == true
    EachDefiner.new(false, 0, nil).any?.should == true
    EachDefiner.new().any?.should == false
    EachDefiner.new(false, nil).any?.should == false    
    EachDefiner.new(true).any?.should == true
  end
end

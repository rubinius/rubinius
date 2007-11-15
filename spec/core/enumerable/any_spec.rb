require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#any?" do
  it "any? with no block should return true if any element is not false or nil" do
    EnumerableSpecs::Numerous.new.any?.should == true 
    EnumerableSpecs::Numerous.new(1, nil, 2).any?.should == true 
    EnumerableSpecs::Numerous.new(false).any?.should == false     
  end
  
  it "any? should return true if the block ever returns other than false or nil" do
    EnumerableSpecs::Numerous.new.any? { |i| i == 5 }.should == true
  end
  
  it "any? should return false if the block never returns other than false or nil" do
    EnumerableSpecs::Numerous.new.any? { false }.should == false
    EnumerableSpecs::Numerous.new.any? { nil }.should == false
  end
  
  it "any? with no block should return true if any element is not false or nil (Legacy form rubycon)" do
    EnumerableSpecs::EachDefiner.new('a','b','c').any?.should == true
    EnumerableSpecs::EachDefiner.new(false, 0, nil).any?.should == true
    EnumerableSpecs::EachDefiner.new().any?.should == false
    EnumerableSpecs::EachDefiner.new(false, nil).any?.should == false    
    EnumerableSpecs::EachDefiner.new(true).any?.should == true
  end
end

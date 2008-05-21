require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#all?" do

  before :each do
    @enum1 = [0, 1, 2, -1]
    @enum2 = [nil, false, true]
  end

  it "fails when have a wrong argument" do
    lambda { @enum1.all?(1) }.should raise_error(ArgumentError)
  end
  
  it "with no block returns true if no elements are false or nil" do
    EnumerableSpecs::Numerous.new.all?.should == true
    EnumerableSpecs::Numerous.new(1, nil, 2).all?.should == false
    EnumerableSpecs::Numerous.new(false).all?.should == false
  
    @enum1.all?.should == true
    @enum2.all?.should == false
  end

  it "returns true if the block never returns false or nil" do
    EnumerableSpecs::Numerous.new.all? { true }.should == true
    @enum1.all?{ |o| o < 5 }.should == true
    @enum1.all?{ |o| o > 2 }.should == false
  end
  
  it "returns false if the block ever returns false or nil" do
    EnumerableSpecs::Numerous.new.all? { |i| i > 5 }.should == false
    EnumerableSpecs::Numerous.new.all? { |i| i == 3 ? nil : true }.should == false
  end
  
  it "passes each element of the collection to the given block. (Legacy form rubycon)" do
    @enum1.all?{ |o| 5.times{ @enum1.shift } }.should == true
    EnumerableSpecs::EachDefiner.new().all?.should == true
    EnumerableSpecs::EachDefiner.new('a','b','c').all?.should == true
    EnumerableSpecs::EachDefiner.new(0, "x", true).all?.should == true
    EnumerableSpecs::EachDefiner.new(nil).all?.should == false
    EnumerableSpecs::EachDefiner.new(nil, nil).all?.should == false
    EnumerableSpecs::EachDefiner.new(false).all?.should == false
    EnumerableSpecs::EachDefiner.new(false, false).all?.should == false
    EnumerableSpecs::EachDefiner.new(0, "x", false, true).all?.should == false
  end

  after :each do
    @enum1 = nil
  end

end

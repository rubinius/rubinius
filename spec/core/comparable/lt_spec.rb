require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Comparable#<" do
  it "returns true if other is greater" do
    (ComparableSpecs::Weird.new(1) < ComparableSpecs::Weird.new(-1)).should == true
  end
  
  it "returns false if other is lesser than or equal" do
    a = ComparableSpecs::Weird.new(-1)
    b = ComparableSpecs::Weird.new(0)

    (a < b).should == false
    (a < a).should == false
    (b < b).should == false
  end
end

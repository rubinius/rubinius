require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Comparable#<=" do
  it "returns true if other is greater than or equal" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(-1)

    (a <= b).should == true
    (a <= a).should == true
    (b <= b).should == true
  end
  
  it "returns false if other is lesser" do
    (ComparableSpecs::Weird.new(-1) <= ComparableSpecs::Weird.new(0)).should == false
  end
end

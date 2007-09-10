require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Comparable#>" do
  it "returns true if other is lesser" do
    (ComparableSpecs::Weird.new(-1) > ComparableSpecs::Weird.new(0)).should == true
  end

  it "returns false if other is greater than or equal" do
    a = ComparableSpecs::Weird.new(-1)
    b = ComparableSpecs::Weird.new(0)

    (b > a).should == false
    (a > a).should == false
    (b > b).should == false
  end
end

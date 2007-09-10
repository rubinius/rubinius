require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Comparable#>=" do
  it "returns true if other is lesser than or equal" do
    a = ComparableSpecs::Weird.new(-1)
    b = ComparableSpecs::Weird.new(0)
    c = ComparableSpecs::Weird.new(1)

    (b <= a).should == true
    (b <= c).should == true
  end

  it "returns false if other is greater" do
    (ComparableSpecs::Weird.new(1) >= ComparableSpecs::Weird.new(-1)).should == false
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Comparable#==" do
  it "returns true if other is equal" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(-1)
    c = ComparableSpecs::Weird.new(1)

    (a == c).should == true
    (a == a).should == true
    (b == b).should == true
    (c == c).should == true
  end

  it "returns false if other is not equal" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(-1)
    c = ComparableSpecs::Weird.new(1)

    (a == b).should == false
    (b == c).should == false
  end
end

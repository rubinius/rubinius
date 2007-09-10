require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Comparable#between?" do
  it "returns true if min <= self <= max" do
    a = ComparableSpecs::Weird.new(-1)
    b = ComparableSpecs::Weird.new(0)
    c = ComparableSpecs::Weird.new(1)

    b.between?(c, a).should == true
    c.between?(b, a).should == true
  end
  
  it "returns false if self < min or self > max" do
    a = ComparableSpecs::Weird.new(-1)
    b = ComparableSpecs::Weird.new(0)
    c = ComparableSpecs::Weird.new(1)

    a.between?(b, c).should == false
    a.between?(c, b).should == false
  end
end

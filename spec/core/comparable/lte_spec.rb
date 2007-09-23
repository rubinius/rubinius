require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Comparable#<=" do
  it "calls #<=> on self with other and returns true if #<=> returns 0 or any Integer less than 0" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(20)
    
    a.should_receive(:<=>, :returning => 0)
    (a <= b).should == true
    
    a.should_receive(:<=>, :returning => 0.0)
    (a <= b).should == true

    a.should_receive(:<=>, :returning => -1)
    (a <= b).should == true

    a.should_receive(:<=>, :returning => -0.1)
    (a <= b).should == true

    a.should_receive(:<=>, :returning => -10000000)
    (a <= b).should == true
  end

  it "returns false if calling #<=> on self returns any Integer greater than 0" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(10)
 
    a.should_receive(:<=>, :returning => 0.1)
    (a <= b).should == false

    a.should_receive(:<=>, :returning => 1.0)
    (a <= b).should == false
    
    a.should_receive(:<=>, :returning => 10000000)
    (a <= b).should == false
  end

  it "raises an ArgumentError if calling #<=> on self returns nil" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(20)
    
    a.should_receive(:<=>, :returning => nil)
    should_raise(ArgumentError, "comparison of ComparableSpecs::Weird with ComparableSpecs::Weird failed") do
      (a <= b)
    end
  end
end
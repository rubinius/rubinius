require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Comparable#==" do
  it "returns true if other is the same as self" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(20)
    
    (a == a).should == true
    (b == b).should == true
  end
  
  it "calls #<=> on self with other and returns true if #<=> returns 0" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(10)
    
    a.should_receive(:<=>, :returning => 0)
    (a == b).should == true

    a.should_receive(:<=>, :returning => 0.0)
    (a == b).should == true
  end
  
  it "returns false if calling #<=> on self returns a non-zero Integer" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(10)
 
    a.should_receive(:<=>, :returning => 1)
    (a == b).should == false
    
    a.should_receive(:<=>, :returning => -1)
    (a == b).should == false
  end
  
  it "returns nil if calling #<=> on self returns nil or a non-Integer" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(10)

    a.should_receive(:<=>, :returning => nil)
    (a == b).should == nil

    a.should_receive(:<=>, :returning => "abc")
    (a == b).should == nil
  end

  it "returns nil if calling #<=> on self raises a StandardError" do
    a = ComparableSpecs::Weird.new(0)
    b = ComparableSpecs::Weird.new(10)
    
    def a.<=>(b) raise StandardError, "test"; end
    (a == b).should == nil

    # TypeError < StandardError
    def a.<=>(b) raise TypeError, "test"; end
    (a == b).should == nil

    def a.<=>(b) raise Exception, "test"; end
    should_raise(Exception) do
      (a == b).should == nil
    end
  end
end

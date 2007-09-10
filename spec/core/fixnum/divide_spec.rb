require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#/" do
  it "returns self divided by other" do
    (2 / 2).should == 1
    (3 / 2).should == 1
  end
  
  it "raises ZeroDivisionError if other is zero and not a Float" do
    should_raise(ZeroDivisionError) { 1 / 0 }
  end
  
  it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
    (1 / 0.0).to_s.should == 'Infinity'
    (-1 / 0.0).to_s.should == '-Infinity'
  end

  it "coerces fixnum and return self divided by other" do
    (-1 / 50.4).to_s.should == "-0.0198412698412698"
    (1 / 0xffffffff).should == 0
  end
end

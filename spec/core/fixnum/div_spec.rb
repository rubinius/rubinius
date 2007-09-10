require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#div" do
  it "returns self divided by other as an Integer" do
    2.div(2).should == 1
    1.div(2).should == 0
  end
  
  it "coerces fixnum and return self divided by other as an Integer" do
    1.div(0.2).should == 5
    -1.div(50.4).should == -1
    1.div(0xffffffff).should == 0
  end
  
  it "raises ZeroDivisionError if other is zero and not a Float" do
    should_raise(ZeroDivisionError) { 1.div(0) }
  end
end

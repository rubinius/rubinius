require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#**" do
  it "returns self raised to the given power" do
    (2 ** 0).should == 1
    (2 ** 1).should == 2
    (2 ** 2).should == 4
    
    (9 ** 0.5).to_s.should == '3.0'
    (5 ** -1).to_f.to_s.should == '0.2'
    
    (2 ** 40).should == 1099511627776
  end
  
  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError, "Object can't be coerced into Fixnum") do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      13 ** obj
    end
    
    should_raise(TypeError, "String can't be coerced into Fixnum") do
      13 ** "10"
    end
  end
end

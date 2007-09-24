require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#>>" do
  it "returns self shifted the given amount of bits to the right" do
    (7 >> 1).should == 3
    (4095 >> 3).should == 511
    (9245278 >> 1).should == 4622639
  end

  it "performs a left-shift if given a negative value" do
    (7 >> -1).should == (7 << 1)
    (4095 >> -3).should == (4095 << 3)
  end
  
  it "tries to convert it's argument to an Integer using to_int" do
    (7 >> 1.3).should == 3
    
    (obj = Object.new).should_receive(:to_int, :returning => 1)
    (7 >> obj).should == 3
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = Object.new
    should_raise(TypeError, "can't convert Object into Integer") { 3 >> obj }
    
    obj.should_receive(:to_int, :returning => "asdf")
    should_raise(TypeError, "Object#to_int should return Integer") { 3 >> obj }
  end
end

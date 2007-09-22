require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#&" do
  it "returns self bitwise AND other" do
    (256 & 16).should == 0
    (2010 & 5).should == 0
    (65535 & 1).should == 1
    (0xffff & 0xffffffff).should == 65535
  end
  
  it "tries to convert it's argument to an Integer using to_int" do
    (3 & 2.4).should == 2
    
    (obj = Object.new).should_receive(:to_int, :returning => 2)
    (3 & obj).should == 2
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = Object.new
    should_raise(TypeError, "can't convert Object into Integer") { 3 & obj }
    
    obj.should_receive(:to_int, :returning => "asdf")
    should_raise(TypeError, "Object#to_int should return Integer") { 3 & obj }
  end
end

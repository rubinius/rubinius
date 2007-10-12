require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#^" do
  it "returns self bitwise EXCLUSIVE OR other" do
    (3 ^ 5).should == 6
    (-7 ^ 15.2).should == -10
    (-2 ^ -255).should == 255
    (5 ^ 0xffffffff).should == 4294967290
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (5 ^ 4.3).should == 1
    
    (obj = Object.new).should_receive(:to_int, :returning => 4)
    (3 ^ obj).should == 7
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = Object.new
    should_raise(TypeError) { 3 ^ obj }
    
    obj.should_receive(:to_int, :returning => "asdf")
    should_raise(TypeError) { 3 ^ obj }
  end
end

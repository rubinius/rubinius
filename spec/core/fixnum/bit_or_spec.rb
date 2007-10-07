require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#|" do
  it "returns self bitwise OR other" do
    (1 | 0).should == 1
    (5 | 4).should == 5
    (5 | 6).should == 7
    (248 | 4096).should == 4344
  end

  it "tries to convert the given argument to an Integer using to_int" do
    (5 | 4.3).should == 5
    
    (obj = Object.new).should_receive(:to_int, :returning => 4)
    (3 | obj).should == 7
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = Object.new
    should_raise(TypeError) { 3 | obj }
    
    obj.should_receive(:to_int, :returning => "asdf")
    should_raise(TypeError) { 3 | obj }
  end
end

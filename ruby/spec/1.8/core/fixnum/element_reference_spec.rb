require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#[]" do
  it "returns the nth bit in the binary representation of self" do
    2[3].should == 0
    15[1].should == 1

    2[3].should == 0
    3[0xffffffff].should == 0
    3[-0xffffffff].should == 0
  end
  
  it "tries to convert the given argument to an Integer using #to_int" do
    15[1.3].should == 15[1]
    
    (obj = mock('1')).should_receive(:to_int).and_return(1)
    2[obj].should == 1
  end

  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { 3[obj] }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { 3[obj] }.should raise_error(TypeError)
  end
end

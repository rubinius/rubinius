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
  
  it "performs a right-shift if given a negative value" do
    (-7 >> 1).should == -4
    (-4095 >> 3).should == -512
  end
  
  it "tries to convert it's argument to an Integer using to_int" do
    (7 >> 1.3).should == 3
    
    (obj = mock('1')).should_receive(:to_int).and_return(1)
    (7 >> obj).should == 3
  end
  
  it "raises a TypeError when the given argument can't be converted to Integer" do
    obj = mock('asdf')
    lambda { 3 >> obj }.should raise_error(TypeError)
    
    obj.should_receive(:to_int).and_return("asdf")
    lambda { 3 >> obj }.should raise_error(TypeError)
  end

  it "does not raise RangeError when the given argument is out of range of Fixnum" do
    (obj1 = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
    (obj2 = mock('large value')).should_receive(:to_int).and_return(8000_0000_0000_0000_0000)
    (3 >> obj1).should == 0
    (-3 >> obj2).should == -1

    obj = 8e19
    (3 >> obj).should == 0
    (-3 >> obj).should == -1
  end


end

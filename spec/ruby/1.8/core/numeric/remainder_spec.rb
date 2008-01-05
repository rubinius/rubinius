require File.dirname(__FILE__) + '/../../spec_helper'

# If num and numeric have different signs, returns mod-numeric;
# otherwise, returns mod. In both cases mod is the value num.modulo(numeric)
describe "Numeric#remainder" do    
  it "remainder the right  integers" do 
    13.remainder(4).should == 1
    13.remainder(-4).should == 1
    -13.remainder(4).should == -1
    -13.remainder(-4).should == -1
  end
  
  it "remainder right  integers and floats" do 
    13.remainder(4.0).should == 1.0
    13.remainder(-4.0).should == 1.0
    -13.remainder(4.0).should == -1.0
    -13.remainder(-4.0).should == -1.0
  end
  
  it "remainder right the integers and floats" do 
    11.5.remainder(4.0).should == 3.5
    11.5.remainder(-4.0).should == 3.5
    -11.5.remainder(4.0).should == -3.5
    -11.5.remainder(-4.0).should == -3.5
  end
  
  it " should remainder right with bignums and integers" do
    (3**33).remainder(-100).should ==  23
    (3**33).remainder(100).should == 23
    (-2**31).remainder(2**31).should == 0
    (-2**31).remainder(2**31).should == 0
  end
  
  it "raises an ArgumentError when not passed one argument" do
    lambda { 13.remainder       }.should raise_error(ArgumentError)
    lambda { 13.remainder(1, 2) }.should raise_error(ArgumentError)
  end
  
  it "raises a ZeroDivisionError when passed 0" do
    lambda { 13.remainder(0) }.should raise_error(ZeroDivisionError)
  end
  
  it "raises a TypeError when not passed a Numeric type" do
    lambda { 13.remainder(nil)    }.should raise_error(TypeError)
    lambda { 13.remainder('test') }.should raise_error(TypeError)
    lambda { 13.remainder(true)   }.should raise_error(TypeError)   
  end   
end

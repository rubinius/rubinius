require File.dirname(__FILE__) + '/../../spec_helper'

# FIXME: this awful wording
describe "Numeric#div" do  
  it "div right  integers" do 
    13.div(4).should == 3
    4.div(13).should == 0
  end
  
  it "div right  integers and floats" do 
    13.div(4.0).should == 3
    4.div(13).should == 0
  end
  
  it "div right the integers and floats" do 
    13.div(4.0).should == 3
    4.div(13).should == 0
  end
  
  it "div right  floats" do 
    13.0.div(4.0).should == 3
    4.0.div(13).should == 0
  end
  
  it "returns self divided by other" do
    (3**33).div(100).should == 55590605665555
  end
  
  it "raises an ArgumentError if not passed one argument" do
    lambda { 13.div       }.should raise_error(ArgumentError)
    lambda { 13.div(1, 2) }.should raise_error(ArgumentError)
  end
  
  it "raises a ZeroDivisionError if passed 0" do
    lambda { 13.div(0) }.should raise_error(ZeroDivisionError)
  end
  
  it "raises a TypeError if not passed a Numeric type" do
    lambda { 13.div(nil)    }.should raise_error(TypeError)
    lambda { 13.div('test') }.should raise_error(TypeError)
    lambda { 13.div(true)   }.should raise_error(TypeError)   
  end
end

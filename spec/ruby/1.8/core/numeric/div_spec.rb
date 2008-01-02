require File.dirname(__FILE__) + '/../../spec_helper'

# Uses / to perform division, then converts the result to an integer
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
  
  it "should div right with bignums and integers" do
    (3**33).div( 100).should == 55590605665555
  end
  
  it "raise the expected exception" do
    lambda { 13.div         }.should raise_error(ArgumentError)
    lambda { 13.div(0)      }.should raise_error(ZeroDivisionError)
    lambda { 13.div(nil)    }.should raise_error(TypeError)
    lambda { 13.div('test') }.should raise_error(TypeError)
    lambda { 13.div(true)   }.should raise_error(TypeError)   
  end
end

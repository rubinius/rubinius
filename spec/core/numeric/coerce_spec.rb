require File.dirname(__FILE__) + '/../../spec_helper'

describe "Numeric#coerce" do   
  before(:each) do 
    @integer = 1
    @float   = 2.5    
  end  
  
  it "coerce integers" do 
    @integer.coerce(@integer).should == [1,1]
    @integer.coerce(@float).should ==[2.5,1]
  end  
  
  it "coerce floats" do 
    @float.coerce(@float).should == [2.5,2.5]
    @float.coerce(@integer).should == [1,2.5]
  end  
  
  it "coerce with 0" do 
    @integer.coerce(0).should == [0,1]
    @float.coerce(0).should == [0,2.5]
  end  
  
  it "coerce with bignums " do 
    @integer.coerce(4294967296).should == [4294967296, 1]
    @float.coerce(4294967296).should == [4294967296.0, 2.5]
  end  
 
  it "should NOT coerce strings to numerics" do
    should_raise(TypeError) { 1 + "2" }
    should_raise(TypeError) { 1 - "2.3" }
    should_raise(TypeError) { 1.2 + "2.3" }
    should_raise(TypeError) { 1 - "1" }
  end  
  
  it "return the vaule if number is different to 0" do 
    should_raise(ArgumentError){ @integer.coerce("test") }
    should_raise(TypeError){ @integer.coerce(nil) }
    should_raise(TypeError){ @integer.coerce(false) }      
    should_raise(ArgumentError){ @integer.coerce }
  end  
end

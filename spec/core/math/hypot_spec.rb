require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Math.hypot" do
  it "returns a float" do
    Math.hypot(3,4).class.should == Float
  end
  
  it "returns the length of the hypotenuse of a right triangle with legs given by the arguments" do 
    Math.hypot(0, 0).should_be_close(0.0, TOLERANCE)
    Math.hypot(2, 10).should_be_close( 10.1980390271856, TOLERANCE)
    Math.hypot(5000, 5000).should_be_close(7071.06781186548, TOLERANCE)
    Math.hypot(0.0001, 0.0002).should_be_close(0.000223606797749979, TOLERANCE)
    Math.hypot(-2, -10).should_be_close(10.1980390271856, TOLERANCE)
    Math.hypot(2, 10).should_be_close(10.1980390271856, TOLERANCE)
  end
    
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.hypot("test","test2") } 
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(ArgumentError) { Math.hypot(nil) }
  end 
end 

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# sine : (-Inf, Inf) --> (-1.0, 1.0)
describe "Math.sin" do 
  it "returns a float" do 
    Math.sin(Math::PI).class.should == Float
  end 
  
  it "returns the sine of the argument expressed in radians" do    
    Math.sin(Math::PI).should_be_close(0.0, TOLERANCE)
    Math.sin(0).should_be_close(0.0, TOLERANCE)
    Math.sin(Math::PI/2).should_be_close(1.0, TOLERANCE)    
    Math.sin(3*Math::PI/2).should_be_close(-1.0, TOLERANCE)
    Math.sin(2*Math::PI).should_be_close(0.0, TOLERANCE)
  end  
 
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.sin("test") } 
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.sin(nil) }
  end  
end

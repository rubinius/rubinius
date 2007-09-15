require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# cosine : (-Inf, Inf) --> (-1.0, 1.0)
describe "Math.cos" do  
  it "returns a float" do 
    Math.cos(Math::PI).class.should == Float
  end 

  it "returns the cosine of the argument expressed in radians" do    
    Math.cos(Math::PI).should_be_close(-1.0, TOLERANCE)
    Math.cos(0).should_be_close(1.0, TOLERANCE)
    Math.cos(Math::PI/2).should_be_close(0.0, TOLERANCE)    
    Math.cos(3*Math::PI/2).should_be_close(0.0, TOLERANCE)
    Math.cos(2*Math::PI).should_be_close(1.0, TOLERANCE)
  end  
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.cos("test") }
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.cos(nil) }
  end
  
  it "accepts any argument that can be coerced with Float()" do
    Math.cos(MathSpecs::Float.new).should_be_close(0.54030230586814, TOLERANCE)
  end
end  

describe "Math#cos" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:cos, 3.1415).should_be_close(-0.999999995707656, TOLERANCE)
  end
end

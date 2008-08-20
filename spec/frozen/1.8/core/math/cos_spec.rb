require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# cosine : (-Inf, Inf) --> (-1.0, 1.0)
describe "Math.cos" do  
  it "returns a float" do 
    Math.cos(Math::PI).class.should == Float
  end 

  it "returns the cosine of the argument expressed in radians" do    
    Math.cos(Math::PI).should be_close(-1.0, TOLERANCE)
    Math.cos(0).should be_close(1.0, TOLERANCE)
    Math.cos(Math::PI/2).should be_close(0.0, TOLERANCE)    
    Math.cos(3*Math::PI/2).should be_close(0.0, TOLERANCE)
    Math.cos(2*Math::PI).should be_close(1.0, TOLERANCE)
  end  
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    lambda { Math.cos("test") }.should raise_error(ArgumentError)
  end
  
  it "raises a TypeError if the argument is nil" do
    lambda { Math.cos(nil) }.should raise_error(TypeError)
  end
  
  it "accepts any argument that can be coerced with Float()" do
    Math.cos(MathSpecs::Float.new).should be_close(0.54030230586814, TOLERANCE)
  end
end  

describe "Math#cos" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:cos, 3.1415).should be_close(-0.999999995707656, TOLERANCE)
  end
end

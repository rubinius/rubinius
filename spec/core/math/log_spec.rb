require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# The natural logarithm, having base Math::E
context "Math.log" do
  it "returns a float" do
    Math.log(1).class.should == Float
  end
  
  it "returns the natural logarithm of the argument" do 
    Math.log(0.0001).should_be_close(-9.21034037197618, TOLERANCE)
    Math.log(0.000000000001e-15).should_be_close(-62.1697975108392, TOLERANCE)
    Math.log(1).should_be_close(0.0, TOLERANCE)
    Math.log(10).should_be_close( 2.30258509299405, TOLERANCE)
    Math.log(10e15).should_be_close(36.8413614879047, TOLERANCE)
  end
  
  it "raises an Errno::EDOM if the argument is less than 0" do    
    should_raise(Errno::EDOM) { Math.log(-1e-15) }
  end 
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.log("test") }
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.log(nil) }
  end
  
  it "accepts any argument that can be coerced with Float()" do
    Math.log(MathSpecs::Float.new).should_be_close(0.0, TOLERANCE)
  end
end

describe "Math#log" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:log, 5.21).should_be_close(1.65057985576528, TOLERANCE)
  end
end

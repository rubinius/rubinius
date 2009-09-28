require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# The natural logarithm, having base Math::E
describe "Math.log" do
  it "returns a float" do
    Math.log(1).class.should == Float
  end
  
  it "returns the natural logarithm of the argument" do 
    Math.log(0.0001).should be_close(-9.21034037197618, TOLERANCE)
    Math.log(0.000000000001e-15).should be_close(-62.1697975108392, TOLERANCE)
    Math.log(1).should be_close(0.0, TOLERANCE)
    Math.log(10).should be_close( 2.30258509299405, TOLERANCE)
    Math.log(10e15).should be_close(36.8413614879047, TOLERANCE)
  end
  
  conflicts_with :Complex do
    it "raises an Errno::EDOM if the argument is less than 0" do    
      lambda { Math.log(-1e-15) }.should raise_error(Errno::EDOM)
    end
  end
  
  ruby_version_is ""..."1.9" do
    it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
      lambda { Math.log("test") }.should raise_error(ArgumentError)
    end
  end
  
  ruby_version_is "1.9" do
    it "raises a TypeError if the argument cannot be coerced with Float()" do    
      lambda { Math.log("test") }.should raise_error(TypeError)
    end
  end

  it "raises a TypeError if the argument is nil" do
    lambda { Math.log(nil) }.should raise_error(TypeError)
  end
  
  it "accepts any argument that can be coerced with Float()" do
    Math.log(MathSpecs::Float.new).should be_close(0.0, TOLERANCE)
  end
end

describe "Math#log" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:log, 5.21).should be_close(1.65057985576528, TOLERANCE)
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# The common logarithm, having base 10
describe "Math.log10" do
  it "returns a float" do 
    Math.log10(1).class.should == Float
  end
  
  it "return the base-10 logarithm of the argument" do
    Math.log10(0.0001).should_be_close(-4.0, TOLERANCE)
    Math.log10(0.000000000001e-15).should_be_close(-27.0, TOLERANCE)
    Math.log10(1).should_be_close(0.0, TOLERANCE)
    Math.log10(10).should_be_close(1.0, TOLERANCE)
    Math.log10(10e15).should_be_close(16.0, TOLERANCE)
  end
  
  it "raises an Errno::EDOM if the argument is less than 0" do
    should_raise( Errno::EDOM) { Math.log10(-1e-15) }
  end
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do
    should_raise(ArgumentError) { Math.log10("test") }
  end

  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.log10(nil) }
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Math.atanh" do
  it "returns a float" do
    Math.atanh(1.0).class.should == Float
  end
  
  it "returns the inverse hyperbolic tangent of the argument" do
    Math.atanh(0.0).should == 0.0
    Math.atanh(-0.0).should == -0.0
    Math.atanh(0.5).should_be_close(0.549306144334055, TOLERANCE)
    Math.atanh(-0.2).should_be_close(-0.202732554054082, TOLERANCE)
  end
  
  it "returns Infinity for 1.0" do
    Math.atanh(1.0).infinite?.should == 1
  end
  
  it "returns -Infinity for -1.0" do
    Math.atanh(-1.0).infinite?.should == -1
  end
  
  it "it raises Errno::EDOM if x < 1" do
    should_raise(Errno::EDOM) { Math.atanh(1.0 + TOLERANCE) }
    should_raise(Errno::EDOM) { Math.atanh(-1.0 - TOLERANCE) }
  end
  
  it "raises ArgumentError if the argument cannot be coerced with Float()" do
    should_raise(ArgumentError) { Math.atanh("test") }
  end

  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.atanh(nil) }
  end
  
  it "accepts any argument that can be coerced with Float()" do
    Math.atanh(MathSpecs::Float.new).infinite?.should == 1
  end
end

describe "Math#atanh" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:atanh, 0.1415).should_be_close(0.14245589281616, TOLERANCE)
  end
end

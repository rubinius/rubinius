require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Math.ldexp" do
  it "returns a float" do
    Math.ldexp(1.0, 2).class.should == Float
  end
  
  it "returns the argument multiplied by 2**n" do
    Math.ldexp(0.0, 0.0).should == 0.0
    Math.ldexp(0.0, 1.0).should == 0.0
    Math.ldexp(-1.25, 2).should_be_close(-5.0, TOLERANCE)
    Math.ldexp(2.1, -3).should_be_close(0.2625, TOLERANCE)
    Math.ldexp(5.7, 4).should_be_close(91.2, TOLERANCE)
  end

  it "raises an ArgumentError if the first argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.ldexp("test", 2) }
  end
  
  it "raises an TypeError if the second argument cannot be coerced with Integer()" do
    should_raise(TypeError) { Math.ldexp(3.2, "this") }
  end
  
  it "raises a TypeError if the first argument is nil" do
    should_raise(TypeError) { Math.ldexp(nil, 2) }
  end
  
  it "raises a TypeError if the second argument is nil" do
    should_raise(TypeError) { Math.ldexp(3.1, nil) }
  end
  
  it "accepts any first argument that can be coerced with Float()" do
    Math.ldexp(MathSpecs::Float.new, 2).should_be_close(4.0, TOLERANCE)
  end
  
  it "accepts any second argument that can be coerced with Integer()" do
    Math.ldexp(3.23, MathSpecs::Integer.new).should_be_close(12.92, TOLERANCE)
  end
end

describe "Math#ldexp" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:ldexp, 3.1415, 2).should_be_close(12.566, TOLERANCE)
  end
end

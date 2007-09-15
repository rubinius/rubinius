require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Math.sinh" do
  it "returns a float" do
    Math.sinh(1.2).class.should == Float
  end
  
  it "returns the hyperbolic sin of the argument" do
    Math.sinh(0.0).should == 0.0
    Math.sinh(-0.0).should == 0.0
    Math.sinh(1.5).should_be_close(2.12927945509482, TOLERANCE)
    Math.sinh(-2.8).should_be_close(-8.19191835423591, TOLERANCE)
  end

  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.sinh("test") } 
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.sinh(nil) }
  end  
  
  it "accepts any argument that can be coerced with Float()" do
    Math.sinh(MathSpecs::Float.new).should_be_close(1.1752011936438, TOLERANCE)
  end
end

describe "Math#sinh" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:sinh, 1.99).should_be_close(3.58941916843202, TOLERANCE)
  end
end

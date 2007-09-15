require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Math.exp" do
  it "returns a float" do
    Math.exp(1.0).class.should == Float
  end
  
  it "returns the base-e exponential of the argument" do
    Math.exp(0.0).should == 1.0
    Math.exp(-0.0).should == 1.0
    Math.exp(-1.8).should_be_close(0.165298888221587, TOLERANCE)
    Math.exp(1.25).should_be_close(3.49034295746184, TOLERANCE)
  end

  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.exp("test") }
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.exp(nil) }
  end
  
  it "accepts any argument that can be coerced with Float()" do
    Math.exp(MathSpecs::Float.new).should_be_close(Math::E, TOLERANCE)
  end
end

describe "Math#exp" do
  it "is accessible as a private instance method" do
    IncludesMath.new.send(:exp, 23.1415).should_be_close(11226018484.0012, TOLERANCE)
  end
end

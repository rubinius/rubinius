require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Math.sqrt" do
  it "returns a float" do
    Math.sqrt(1).class.should == Float
  end
  
  it "returns the square root of the argument" do
    Math.sqrt(1).should == 1.0
    Math.sqrt(4.0).should == 2.0
    Math.sqrt(15241578780673814.441547445).to_s.should == '123456789.123457'
  end

  it "raises an ArgumentError if the argument cannot be coerced with Float()" do
    should_raise(ArgumentError) { Math.sqrt("test") }
  end

  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.sqrt(nil) }
  end    

  it "is accessible as a private instance method" do
    IncludesMath.new.send(:sqrt, 1).should == 1
    should_raise(NoMethodError) { IncludesMath.new.sqrt(1) }
  end
end

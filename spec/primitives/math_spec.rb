require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The math_sqrt primitive sent a float" do
  specify "should return the square root of its argument" do
    Math.prim.math_sqrt(25.0).should == 5.0
    Math.prim.math_sqrt(49.0).should == 7.0
  end
end

context "The math_sqrt primitive sent a negative float" do
  specify "should return NaN (Not a Number)" do
    Math.prim.math_sqrt(-30.0).nan?.should == true
    Math.prim.math_sqrt(-0.03).nan?.should == true
  end
end

context "The math_sqrt primitive sent a non-float" do
  specify "should raise a PrimitiveFailure" do
    Math.prim.math_sqrt(3).should_raise(PrimitiveFailure)
    Math.prim.math_sqrt(44).should_raise(PrimitiveFailure)
  end
end

context "The math_sqrt primitive sent the wrong number of args" do
  specify "should raise an ArgumentError if there are more than 1 args" do
    Math.prim.math_sqrt(3.0, 4.0).should_raise(ArgumentError) # 2 args
    Math.prim.math_sqrt(3.0, 4.0, 5.0).should_raise(ArgumentError) # 3 args
  end

  specify "should raise an ArgumentError if there are less than 1 args" do
    Math.prim.math_sqrt.should_raise(ArgumentError) # 0 args
  end
end


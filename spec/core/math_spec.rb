require File.dirname(__FILE__) + '/../spec_helper'


# acos, acos, acos!, acosh, acosh, acosh!, asin, asin, asin!, asinh, 
# asinh, asinh!, atan, atan, atan!, atan2, atan2, atan2!, atanh, 
# atanh, atanh!, cos, cos, cos!, cosh, cosh, cosh!, erf, erfc, exp, 
# exp, exp!, frexp, hypot, ldexp, log, log, log!, log10, log10, 
# log10!, sin, sin, sin!, sinh, sinh, sinh!, sqrt, sqrt, sqrt!, tan, 
# tan, tan!, tanh, tanh, tanh!

TOLERANCE=0.00003

context "Math constants" do
  specify "Constant PI should approximate the value of pi" do
    example do
      Math::PI
    end.should_be_close 3.14159_26535_89793_23846, TOLERANCE
  end
  
  specify "Constant E should approximate the value of Napier's constant" do
    example do
      Math::E
    end.should_be_close 2.71828_18284_59045_23536, TOLERANCE
  end
end

context "Math methods" do
  specify "sqrt should return the square root of parameter" do
    example do
      [Math.sqrt(1), Math.sqrt(4.0), Math.sqrt(15241578780673814.441547445)].inspect
    end.should == "[1.0, 2.0, 123456789.123457]"
  end
end

context "Math included in another object" do
  specify "should make the Math methods available to the object" do
    example do
      class Foo
        include Math
      end
      Foo.new.sqrt(1)
    end.should == 1
  end

  specify "should make the Math constants available to the object" do
    example do
      class Foo
        include Math
      end
      Foo::PI
    end.should_be_close 2.71828_18284_59045_23536, TOLERANCE
  end
end


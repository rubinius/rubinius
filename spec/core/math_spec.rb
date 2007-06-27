require File.dirname(__FILE__) + '/../spec_helper'


# acos,  asin,
# atan, atanh,  cos,  erf, erfc, hypot, log,  log10, 
# log10!, sin, sqrt, sqrt, tan, tan

# TODO   acosh   acosh! asinh asinh asinh!     
# atan2 atan2   atan2! atanh atanh atanh!    cosh   cosh   cosh!      
# exp exp exp! frexp ldexp sinh sinh sinh!  tanh   tanh   tanh!  


TOLERANCE=0.00003

class IncludesMath
  include Math
end

class ClassLikeFloat
  def to_f
    1.0
  end
end

describe "Math constant" do
  specify "PI  approximates the value of pi" do
    Math::PI.should_be_close(3.14159_26535_89793_23846, TOLERANCE)
  end
  
  specify "E approximates the value of Napier's constant" do
    Math::E.should_be_close(2.71828_18284_59045_23536, TOLERANCE)
  end
end

describe "A class that includes the Math module" do
  it "can access the sqrt method as a private instance method" do
    IncludesMath.new.send(:sqrt, 1).should == 1
    should_raise(NoMethodError) { IncludesMath.new.sqrt(1) }
  end
  
  it "can access the cos method as a private instance method" do
    IncludesMath.new.send(:cos, 0).should == 1.0
    should_raise(NoMethodError) { IncludesMath.new.sqrt(0) }
  end
  
  # TODO: specs for the remainder of the Math methods accessible
  # as private instance methods of the class that includes Math

  it "has the Math::E constant" do
    IncludesMath::E.should == Math::E
  end

  it "has the Math::PI constant" do
    IncludesMath::PI.should == Math::PI
  end
end  

# cosine : (-Inf, Inf) --> (-1.0, 1.0)
describe "Math.cos" do  
  it "returns a float" do 
    Math.cos(Math::PI).class.should == Float
  end 

  it "returns the cosine of the argument expressed in radians" do    
    Math.cos(Math::PI).should_be_close(-1.0, TOLERANCE)
    Math.cos(0).should_be_close(1.0, TOLERANCE)
    Math.cos(Math::PI/2).should_be_close(0.0, TOLERANCE)    
    Math.cos(3*Math::PI/2).should_be_close(0.0, TOLERANCE)
    Math.cos(2*Math::PI).should_be_close(1.0, TOLERANCE)
  end  
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.cos("test") }
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.cos(nil) }
  end
  
  it "accepts any argument that can be coerced with Float()" do
    Math.cos(ClassLikeFloat.new).should_be_close(0.54030230586814, TOLERANCE)
  end
end  

# sine : (-Inf, Inf) --> (-1.0, 1.0)
describe "Math.sin" do 
  it "returns a float" do 
    Math.sin(Math::PI).class.should == Float
  end 
  
  it "returns the sine of the argument expressed in radians" do    
    Math.sin(Math::PI).should_be_close(0.0, TOLERANCE)
    Math.sin(0).should_be_close(0.0, TOLERANCE)
    Math.sin(Math::PI/2).should_be_close(1.0, TOLERANCE)    
    Math.sin(3*Math::PI/2).should_be_close(-1.0, TOLERANCE)
    Math.sin(2*Math::PI).should_be_close(0.0, TOLERANCE)
  end  
 
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.sin("test") } 
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.sin(nil) }
  end  
end

# arccosine : (-1.0, 1.0) --> (0, PI)	 	                
describe "Math.acos" do  
  it "returns a float" do 
    Math.acos(1).class.should == Float 
  end 
  
  it "returns the arccosine of the argument" do 
    Math.acos(1).should_be_close(0.0, TOLERANCE) 
    Math.acos(0).should_be_close(1.5707963267949, TOLERANCE) 
    Math.acos(-1).should_be_close(Math::PI,TOLERANCE) 
    Math.acos(0.25).should_be_close(1.31811607165282, TOLERANCE) 
    Math.acos(0.50).should_be_close(1.0471975511966 , TOLERANCE) 
    Math.acos(0.75).should_be_close(0.722734247813416, TOLERANCE) 
  end  
  
  it "raises an Errno::EDOM if the argument is greater than 1.0" do    
    should_raise(Errno::EDOM) { Math.acos(1.0001) }
  end  
  
  it "raises an Errno::EDOM if the argument is less than -1.0" do    
    should_raise(Errno::EDOM) { Math.acos(-1.0001) }
  end
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.acos("test") }
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.acos(nil) }
  end  
end

# arcsine : (-1.0, 1.0) --> (-PI/2, PI/2)
describe "Math.asin" do
  it "return a float" do 
    Math.asin(1).class.should == Float
  end 
  
  it "returns the arcsine of the argument" do   
    Math.asin(1).should_be_close(Math::PI/2, TOLERANCE)
    Math.asin(0).should_be_close(0.0, TOLERANCE)
    Math.asin(-1).should_be_close(-Math::PI/2, TOLERANCE)
    Math.asin(0.25).should_be_close(0.252680255142079, TOLERANCE)
    Math.asin(0.50).should_be_close(0.523598775598299, TOLERANCE)
    Math.asin(0.75).should_be_close(0.8480620789814816,TOLERANCE) 
  end
  
  it "raises an Errno::EDOM if the argument is greater than 1.0" do    
    should_raise( Errno::EDOM) { Math.asin(1.0001) }
  end
  
  it "raises an Errno::EDOM if the argument is less than -1.0" do    
    should_raise( Errno::EDOM) { Math.asin(-1.0001) }
  end
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.sin("test") }
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.asin(nil) }
  end    
end

# arctangent : (-Inf, Inf) --> (-PI/2, PI/2)
describe "Math.atan" do     
  it "returns a float" do 
    Math.atan(1).class.should == Float
  end 
  
  it "return the arctangent of the argument" do    
    Math.atan(1).should_be_close(Math::PI/4, TOLERANCE)
    Math.atan(0).should_be_close(0.0, TOLERANCE)
    Math.atan(-1).should_be_close(-Math::PI/4, TOLERANCE)
    Math.atan(0.25).should_be_close(0.244978663126864, TOLERANCE)
    Math.atan(0.50).should_be_close(0.463647609000806, TOLERANCE)
    Math.atan(0.75).should_be_close(0.643501108793284, TOLERANCE)
  end   
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.atan("test") } 
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.atan(nil) }
  end  
end 
  
describe "Math.hypot" do
  it "returns a float" do
    Math.hypot(3,4).class.should == Float
  end
  
  it "returns the length of the hypotenuse of a right triangle with legs given by the arguments" do 
    Math.hypot(0, 0).should_be_close(0.0, TOLERANCE)
    Math.hypot(2, 10).should_be_close( 10.1980390271856, TOLERANCE)
    Math.hypot(5000, 5000).should_be_close(7071.06781186548, TOLERANCE)
    Math.hypot(0.0001, 0.0002).should_be_close(0.000223606797749979, TOLERANCE)
    Math.hypot(-2, -10).should_be_close(10.1980390271856, TOLERANCE)
    Math.hypot(2, 10).should_be_close(10.1980390271856, TOLERANCE)
  end
    
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.hypot("test","test2") } 
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(ArgumentError) { Math.hypot(nil) }
  end 
end 

# erf method is the "error function" encountered in integrating the normal 
# distribution (which is a normalized form of the Gaussian function).
describe "Math.erf" do
  it "returns a float" do 
    Math.erf(1).class.should == Float
  end 
  
  it "returns the error function of the argument" do 
    Math.erf(0).should_be_close(0.0, TOLERANCE)
    Math.erf(1).should_be_close(0.842700792949715, TOLERANCE)
    Math.erf(-1).should_be_close(-0.842700792949715, TOLERANCE)
    Math.erf(0.5).should_be_close(0.520499877813047, TOLERANCE)
    Math.erf(-0.5).should_be_close(-0.520499877813047, TOLERANCE)
    Math.erf(10000).should_be_close(1.0, TOLERANCE)
    Math.erf(-10000).should_be_close(-1.0, TOLERANCE)
    Math.erf(0.00000000000001).should_be_close(0.0, TOLERANCE)
    Math.erf(-0.00000000000001).should_be_close(0.0, TOLERANCE) 
  end
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.erf("test") }
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.erf(nil) }
  end 
end

# erfc is the complementary error function  
describe "Math#erfc" do
  it "returns a float" do
    Math.erf(1).class.should == Float
  end
  
  it "returns the complimentary error function of the argument" do
    Math.erfc(0).should_be_close(1.0, TOLERANCE)
    Math.erfc(1).should_be_close(0.157299207050285, TOLERANCE)
    Math.erfc(-1).should_be_close(1.84270079294971, TOLERANCE)
    Math.erfc(0.5).should_be_close(0.479500122186953, TOLERANCE)
    Math.erfc(-0.5).should_be_close(1.52049987781305, TOLERANCE)
    Math.erfc(10000).should_be_close(0.0, TOLERANCE)
    Math.erfc(-10000).should_be_close(2.0, TOLERANCE)
    Math.erfc(0.00000000000001).should_be_close(0.999999999999989, TOLERANCE)
    Math.erfc(-0.00000000000001).should_be_close(1.00000000000001, TOLERANCE) 
  end  
  
  it "raises an ArgumentError if the argument cannot be coerced with Float()" do    
    should_raise(ArgumentError) { Math.erfc("test") } 
  end
  
  it "raises a TypeError if the argument is nil" do
    should_raise(TypeError) { Math.erfc(nil) }
  end 
end

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
end

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
end

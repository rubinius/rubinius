require File.dirname(__FILE__) + '/../fixtures/classes'
require 'mathn'

shared :mathn_math_rsqrt do |obj|
  describe "Math#{obj == Math ? '.' : '#'}rsqrt" do
    it "returns the square root for Rational numbers" do
      obj.send(:rsqrt, Rational(9, 25)).should == Rational(3, 5)
      obj.send(:rsqrt, 16/64).should == Rational(1, 2)
    end

    it "returns the square root for positive numbers" do
      obj.send(:rsqrt, 1).should == 1
      obj.send(:rsqrt, 4.0).should == 2.0
      obj.send(:rsqrt, 12.34).should == Math.sqrt!(12.34)
    end

    it "raises an Errno::EDOM if the argument is a negative number" do    
      lambda { Math.rsqrt(-1) }.should raise_error(Errno::EDOM)
      lambda { Math.rsqrt(-4.0) }.should raise_error(Errno::EDOM)
      lambda { Math.rsqrt(-16/64) }.should raise_error(Errno::EDOM)
    end
  end
end

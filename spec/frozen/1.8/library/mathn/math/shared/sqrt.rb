require File.dirname(__FILE__) + '/../fixtures/classes'
require 'mathn'

shared :mathn_math_sqrt do |obj|
  describe "Math#{obj == Math ? '.' : '#'}sqrt" do
    it "returns the square root for Rational numbers" do
      obj.send(:sqrt, Rational(9, 25)).should == Rational(3, 5)
      obj.send(:sqrt, 16/64).should == Rational(1, 2)
    end

    it "returns the square root for Complex numbers" do
      obj.send(:sqrt, Complex(1, 0)).should == 1
    end

    it "returns the square root for positive numbers" do
      obj.send(:sqrt, 1).should == 1
      obj.send(:sqrt, 4.0).should == 2.0
      obj.send(:sqrt, 12.34).should == Math.sqrt!(12.34)
    end

    it "returns the square root for negative numbers" do
      obj.send(:sqrt, -9).should == Complex(0, 3)
      obj.send(:sqrt, -5.29).should == Complex(0, 2.3)
      obj.send(:sqrt, -16/64).should == Complex(0, 1/2)
    end
  end
end

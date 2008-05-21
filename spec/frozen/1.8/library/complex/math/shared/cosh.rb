require 'complex'
require File.dirname(__FILE__) + '/../fixtures/classes'

shared :complex_math_cosh do |obj|
  describe "Math#{obj == Math ? '.' : '#'}cosh" do
    it "returns the hyperbolic cosine of the passed argument" do
      obj.send(:cosh, 0.0).should == 1.0
      obj.send(:cosh, -0.0).should == 1.0
      obj.send(:cosh, 1.5).should be_close(2.35240961524325, TOLERANCE)
      obj.send(:cosh, -2.99).should be_close(9.96798496414416, TOLERANCE)
    end

    it "returns the hyperbolic cosine for Complex numbers" do
      obj.send(:cosh, Complex(0, Math::PI)).should be_close(Complex(-1.0, 0.0), TOLERANCE)
      obj.send(:cosh, Complex(3, 4)).should be_close(Complex(-6.58066304055116, -7.58155274274654), TOLERANCE)
    end
  end
end

shared :complex_math_cosh_bang do |obj|
  describe "Math#{obj == Math ? '.' : '#'}cosh!" do
    it "returns the hyperbolic cosine of the passed argument" do
      obj.send(:cosh!, 0.0).should == 1.0
      obj.send(:cosh!, -0.0).should == 1.0
      obj.send(:cosh!, 1.5).should be_close(2.35240961524325, TOLERANCE)
      obj.send(:cosh!, -2.99).should be_close(9.96798496414416, TOLERANCE)
    end

    it "raises a TypeError when passed a Complex number" do
      lambda { obj.send(:cosh!, Complex(4, 5)) }.should raise_error(TypeError)
    end
  end
end
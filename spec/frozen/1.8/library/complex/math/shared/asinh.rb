require 'complex'
require File.dirname(__FILE__) + '/../fixtures/classes'

shared :complex_math_asinh do |obj|
  describe "Math#{obj == Math ? '.' : '#'}asinh" do
    it "returns the inverse hyperbolic sin of the argument" do
      obj.send(:asinh, 1.5).should be_close(1.19476321728711, TOLERANCE)
      obj.send(:asinh, -2.97).should be_close(-1.8089166921397, TOLERANCE)
      obj.send(:asinh, 0.0).should == 0.0
      obj.send(:asinh, -0.0).should == -0.0
      obj.send(:asinh, 1.05367e-08).should be_close(1.05367e-08, TOLERANCE)
      obj.send(:asinh, -1.05367e-08).should be_close(-1.05367e-08, TOLERANCE)
    end
    
    it "returns the inverse hyperbolic sin for Complex numbers" do
      obj.send(:asinh, Complex(3, 4)).should be_close(Complex(2.29991404087927, 0.917616853351479), TOLERANCE)
      obj.send(:asinh, Complex(3.5, -4)).should be_close(Complex(2.36263337274419, -0.843166327537659), TOLERANCE)
    end
  end
end

shared :complex_math_asinh_bang do |obj|
  describe "Math#{obj == Math ? '.' : '#'}asinh!" do
    it "returns the inverse hyperbolic sin of the argument" do
      obj.send(:asinh!, 1.5).should be_close(1.19476321728711, TOLERANCE)
      obj.send(:asinh!, -2.97).should be_close(-1.8089166921397, TOLERANCE)
      obj.send(:asinh!, 0.0).should == 0.0
      obj.send(:asinh!, -0.0).should == -0.0
      obj.send(:asinh!, 1.05367e-08).should be_close(1.05367e-08, TOLERANCE)
      obj.send(:asinh!, -1.05367e-08).should be_close(-1.05367e-08, TOLERANCE)
    end

    it "raises a TypeError when passed a Complex number" do
      lambda { obj.send(:asinh!, Complex(4, 5)) }.should raise_error(TypeError)
    end
  end
end
require 'complex'
require File.dirname(__FILE__) + '/../fixtures/classes'

shared :complex_math_sin do |obj|
  describe "Math#{obj == Math ? '.' : '#'}sin" do
    it "returns the sine of the passed argument expressed in radians" do    
      obj.send(:sin, Math::PI).should be_close(0.0, TOLERANCE)
      obj.send(:sin, 0).should be_close(0.0, TOLERANCE)
      obj.send(:sin, Math::PI/2).should be_close(1.0, TOLERANCE)    
      obj.send(:sin, 3*Math::PI/2).should be_close(-1.0, TOLERANCE)
      obj.send(:sin, 2*Math::PI).should be_close(0.0, TOLERANCE)
    end

    it "returns the sine for Complex numbers" do
      obj.send(:sin, Complex(0, Math::PI)).should be_close(Complex(0.0, 11.5487393572577), TOLERANCE)
      obj.send(:sin, Complex(3, 4)).should be_close(Complex(3.85373803791938, -27.0168132580039), TOLERANCE)
    end
  end
end

shared :complex_math_sin_bang do |obj|
  describe "Math#{obj == Math ? '.' : '#'}sin!" do
    it "returns the sine of the passed argument expressed in radians" do    
      obj.send(:sin!, Math::PI).should be_close(0.0, TOLERANCE)
      obj.send(:sin!, 0).should be_close(0.0, TOLERANCE)
      obj.send(:sin!, Math::PI/2).should be_close(1.0, TOLERANCE)    
      obj.send(:sin!, 3*Math::PI/2).should be_close(-1.0, TOLERANCE)
      obj.send(:sin!, 2*Math::PI).should be_close(0.0, TOLERANCE)
    end

    it "raises a TypeError when passed a Complex number" do
      lambda { obj.send(:sin!, Complex(4, 5)) }.should raise_error(TypeError)
    end
  end
end
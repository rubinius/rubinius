require 'complex'
require File.dirname(__FILE__) + '/../fixtures/classes'

shared :complex_math_tanh do |obj|
  describe "Math#{obj == Math ? '.' : '#'}tanh" do
    it "returns the hyperbolic tangent of the argument" do
      obj.send(:tanh, 0.0).should == 0.0
      obj.send(:tanh, -0.0).should == -0.0
      obj.send(:tanh, 1.0/0.0).should == 1.0
      obj.send(:tanh, 1.0/-0.0).should == -1.0
      obj.send(:tanh, 2.5).should be_close(0.98661429815143, TOLERANCE)
      obj.send(:tanh, -4.892).should be_close(-0.999887314427707, TOLERANCE)
    end
    
    it "returns the hyperbolic tangent for Complex numbers" do
      obj.send(:tanh, Complex(0, Math::PI)).should be_close(Complex(0.0, -1.22464679914735e-16), TOLERANCE)
      obj.send(:tanh, Complex(3, 4)).should be_close(Complex(1.00070953606723, 0.00490825806749599), TOLERANCE)
    end
  end
end

shared :complex_math_tanh_bang do |obj|
  describe "Math#{obj == Math ? '.' : '#'}tanh!" do
    it "returns the hyperbolic tangent of the argument" do
      obj.send(:tanh!, 0.0).should == 0.0
      obj.send(:tanh!, -0.0).should == -0.0
      obj.send(:tanh!, 1.0/0.0).should == 1.0
      obj.send(:tanh!, 1.0/-0.0).should == -1.0
      obj.send(:tanh!, 2.5).should be_close(0.98661429815143, TOLERANCE)
      obj.send(:tanh!, -4.892).should be_close(-0.999887314427707, TOLERANCE)
    end

    it "raises a TypeError when passed a Complex number" do
      lambda { obj.send(:tanh!, Complex(4, 5)) }.should raise_error(TypeError)
    end
  end
end
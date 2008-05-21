require 'complex'

shared :complex_conjugate do |cmd|
  describe "Complex##{cmd}" do
    it "returns the complex conjugate: conj a + bi = a - bi" do
      Complex(3, 5).send(cmd).should == Complex(3, -5)
      Complex(3, -5).send(cmd).should == Complex(3, 5)
      Complex(-3.0, 5.2).send(cmd).should be_close(Complex(-3.0, -5.2), TOLERANCE)
      Complex(3.0, -5.2).send(cmd).should be_close(Complex(3.0, 5.2), TOLERANCE)
    end
  end
end
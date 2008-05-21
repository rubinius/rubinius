require 'complex'

shared :complex_image do |cmd|
  describe "Complex##{cmd}" do
    it "returns the imaginary part of self" do
      Complex(1, 0).send(cmd).should == 0
      Complex(2, 1).send(cmd).should == 1
      Complex(6.7, 8.9).send(cmd).should == 8.9
      Complex(1, bignum_value).send(cmd).should == bignum_value
    end
  end
end
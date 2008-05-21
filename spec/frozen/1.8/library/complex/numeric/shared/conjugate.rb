require 'complex'

shared :numeric_conjugate do |cmd|
  describe "Numeric##{cmd}" do
    it "returns self" do
      20.send(cmd).should == 20
      (-4.5).send(cmd).should == -4.5
      bignum_value.send(cmd).should == bignum_value
    end
  end
end
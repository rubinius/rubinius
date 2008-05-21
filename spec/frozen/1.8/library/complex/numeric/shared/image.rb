require 'complex'

shared :numeric_image do |cmd|
  describe "Numeric##{cmd}" do
    it "returns 0" do
      20.send(cmd).should == 0
      (-4.5).send(cmd).should == 0
      bignum_value.send(cmd).should == 0
    end
  end
end
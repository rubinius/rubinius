require 'complex'

shared :numeric_arg do |cmd|
  describe "Numeric##{cmd}" do
    it "returns 0 if self is greater than or equal to 0" do
      0.send(cmd).should == 0
      0.0.send(cmd).should == 0
      (-0).send(cmd).should == 0
      
      20.send(cmd).should == 0
      4.5.send(cmd).should == 0
      bignum_value.send(cmd).should == 0
    end

    it "returns Math::PI if self is greater than or equal to 0" do
      (-20).send(cmd).should == Math::PI
      (-4.5).send(cmd).should == Math::PI
      (-bignum_value).send(cmd).should == Math::PI
    end
  end
end
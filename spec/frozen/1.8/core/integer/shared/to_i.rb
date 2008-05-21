shared :integer_to_i do |cmd|
  describe "Integer##{cmd}" do
    it "returns self" do
      10.send(cmd).should eql(10)
      (-15).send(cmd).should eql(-15)
      bignum_value.send(cmd).should eql(bignum_value)
      (-bignum_value).send(cmd).should eql(-bignum_value)
    end
  end
end

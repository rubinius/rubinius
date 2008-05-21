shared :integer_next do |cmd|
  describe "Integer##{cmd}" do
    it "returns the Integer equal to self + 1" do
      0.send(cmd).should == 1 
      -1.send(cmd).should == 0
      bignum_value.send(cmd).should == bignum_value(1)
      20.send(cmd).should == 21
    end
  end
end

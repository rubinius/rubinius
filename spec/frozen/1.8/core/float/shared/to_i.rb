shared :float_to_i do |cmd|
  describe "Float##{cmd}" do
    it "returns self truncated to an Integer" do
      899.2.send(cmd).should == 899
      -1.122256e-45.send(cmd).should == 0
      5_213_451.9201.send(cmd).should == 5213451
      1.233450999123389e+12.send(cmd).should == 1233450999123
      -9223372036854775808.1.send(cmd).should == -9223372036854775808
      9223372036854775808.1.send(cmd).should == 9223372036854775808
    end
  end
end

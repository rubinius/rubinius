require 'bigdecimal'

shared :bigdecimal_modulo do |cmd|

  describe "BigDecimal##{cmd}" do

    it "returns self modulo other" do
      bd6543 = BigDecimal.new("6543.21")
      bd5667 = BigDecimal.new("5667.19")
      a = BigDecimal("1.0000000000000000000000000000000000000000005")
      b = BigDecimal("1.00000000000000000000000000000000000000000005")

      bd6543.send(cmd, 137).should == BigDecimal("104.21")
      bd5667.send(cmd, bignum_value).should == 5667.19
      bd6543.send(cmd, BigDecimal("137.24")).should == BigDecimal("92.93")
      bd6543.send(cmd, 137).should be_close(6543.21.%(137), TOLERANCE)
      bd6543.send(cmd, 137).should == bd6543 % 137
      bd5667.send(cmd, bignum_value).should 
        be_close(5667.19.%(0xffffffff), TOLERANCE)
      bd5667.send(cmd, bignum_value).should == bd5667.%(0xffffffff)
      bd6543.send(cmd, 137.24).should be_close(6543.21.%(137.24), TOLERANCE)
      a.send(cmd, b).should == BigDecimal("0.45E-42")
    end

    it "does NOT raise ZeroDivisionError if other is zero" do
      bd6543 = BigDecimal.new("6543.21")
      bd5667 = BigDecimal.new("5667.19")
      a = BigDecimal("1.0000000000000000000000000000000000000000005")
      b = BigDecimal("1.00000000000000000000000000000000000000000005")

      bd5667.send(cmd, 0).to_s.should == 'NaN'
      bd5667.send(cmd, 0.0).to_s.should == 'NaN'
      bd5667.send(cmd, BigDecimal("0")).to_s.should == 'NaN'
      # to_s because: NaN isn't even equal to its self.
    end
  end
end

require 'bigdecimal'

shared :bigdecimal_mult do |cmd, *args|
  describe "BigDecimal##{cmd}" do
    before :each do
      @zero = BigDecimal "0"
      @zero_pos = BigDecimal "+0"
      @zero_neg = BigDecimal "-0"

      @one = BigDecimal "1"
      @mixed = BigDecimal "1.23456789"
      @pos_int = BigDecimal "2E5555"
      @neg_int = BigDecimal "-2E5555"
      @pos_frac = BigDecimal "2E-9999"
      @neg_frac = BigDecimal "-2E-9999"
      @nan = BigDecimal "NaN"
      @infinity = BigDecimal "Infinity"
      @infinity_minus = BigDecimal "-Infinity"
      @one_minus = BigDecimal "-1"
      @frac_1 = BigDecimal "1E-99999"
      @frac_2 = BigDecimal "0.9E-99999"

      @e3_minus = BigDecimal "3E-20001"
      @e = BigDecimal "1.00000000000000000000123456789"
      @tolerance = @e.sub @one, 1000
      @tolerance2 = BigDecimal "30001E-20005"

      @special_vals = [@infinity, @infinity_minus, @nan]
      @regular_vals = [ @one, @mixed, @pos_int, @neg_int,
                        @pos_frac, @neg_frac, @one_minus,
                        @frac_1, @frac_2
                      ]
      @zeroes = [@zero, @zero_pos, @zero_neg]
    end

    it "returns zero of appropriate sign if self or argument is zero" do
      @zero.send(cmd, @zero, *args).sign.should == BigDecimal::SIGN_POSITIVE_ZERO
      @zero_neg.send(cmd, @zero_neg, *args).sign.should == BigDecimal::SIGN_POSITIVE_ZERO
      @zero.send(cmd, @zero_neg, *args).sign.should == BigDecimal::SIGN_NEGATIVE_ZERO
      @zero_neg.send(cmd, @zero, *args).sign.should == BigDecimal::SIGN_NEGATIVE_ZERO

      @one.send(cmd, @zero, *args).sign.should == BigDecimal::SIGN_POSITIVE_ZERO
      @one.send(cmd, @zero_neg, *args).sign.should == BigDecimal::SIGN_NEGATIVE_ZERO

      @zero.send(cmd, @one, *args).sign.should == BigDecimal::SIGN_POSITIVE_ZERO
      @zero.send(cmd, @one_minus, *args).sign.should == BigDecimal::SIGN_NEGATIVE_ZERO
      @zero_neg.send(cmd, @one_minus, *args).sign.should == BigDecimal::SIGN_POSITIVE_ZERO
      @zero_neg.send(cmd, @one, *args).sign.should == BigDecimal::SIGN_NEGATIVE_ZERO
    end

    it "returns NaN if NaN is involved" do
      values = @regular_vals + @zeroes

      values.each do |val|
        @nan.send(cmd, val, *args).nan?.should == true
        val.send(cmd, @nan, *args).nan?.should == true
      end
    end

    it "returns zero if self or argument is zero" do
      values = @regular_vals + @zeroes

      values.each do |val|
        @zeroes.each do |zero|
          zero.send(cmd, val, *args).should == 0
          zero.send(cmd, val, *args).zero?.should == true
          val.send(cmd, zero, *args).should == 0
          val.send(cmd, zero, *args).zero?.should == true
        end
      end
    end
    
    it "returns infinite value if self or argument is infinite" do
      values = @regular_vals
      infs = [@infinity, @infinity_minus]

      values.each do |val|
        infs.each do |inf|
          inf.send(cmd, val, *args).finite?.should == false
          val.send(cmd, inf, *args).finite?.should == false
        end
      end

      @infinity.send(cmd, @infinity, *args).infinite?.should == 1
      @infinity_minus.send(cmd, @infinity_minus, *args).infinite?.should == 1
      @infinity.send(cmd, @infinity_minus, *args).infinite?.should == -1
      @infinity_minus.send(cmd, @infinity, *args).infinite?.should == -1
      @infinity.send(cmd, @one, *args).infinite?.should == 1
      @infinity_minus.send(cmd, @one, *args).infinite?.should == -1
    end

    it "returns NaN if the result is undefined" do
      @zero.send(cmd, @infinity, *args).nan?.should == true
      @zero.send(cmd, @infinity_minus, *args).nan?.should == true
      @infinity.send(cmd, @zero, *args).nan?.should == true
      @infinity_minus.send(cmd, @zero, *args).nan?.should == true
    end

  end
end

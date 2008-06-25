require 'bigdecimal'

shared :bigdecimal_modulo do |cmd, mode|

  describe "BigDecimal##{cmd}" do
    before(:each) do
      @one = BigDecimal("1")
      @zero = BigDecimal("0")
      @zero_pos = BigDecimal("+0")
      @zero_neg = BigDecimal("-0")
      @two = BigDecimal("2")
      @three = BigDecimal("3")
      @mixed = BigDecimal("1.23456789")
      @nan = BigDecimal("NaN")
      @infinity = BigDecimal("Infinity")
      @infinity_minus = BigDecimal("-Infinity")
      @one_minus = BigDecimal("-1")
      @frac_1 = BigDecimal("1E-9999")
      @frac_2 = BigDecimal("0.9E-9999")
    end

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
      bd5667.send(cmd, bignum_value).should be_close(5667.19.%(0xffffffff), TOLERANCE)
      bd5667.send(cmd, bignum_value).should == bd5667.%(0xffffffff)
      bd6543.send(cmd, 137.24).should be_close(6543.21.%(137.24), TOLERANCE)
      a.send(cmd, b).should == BigDecimal("0.45E-42")
      @zero.send(cmd, @one).should == @zero
      @zero.send(cmd, @one_minus).should == @zero
      @two.send(cmd, @one).should == @zero
      @one.send(cmd, @two).should == @one
      @frac_1.send(cmd, @one).should == @frac_1
      @frac_2.send(cmd, @one).should == @frac_2
      @one_minus.send(cmd, @one_minus).should == @zero
      @one_minus.send(cmd, @one).should == @zero
      @one_minus.send(cmd, @two).should == @one
      @one.send(cmd, -@two).should == -@one

      @one_minus.modulo(BigDecimal('0.9')).should == BigDecimal('0.8')
      @one.modulo(BigDecimal('-0.9')).should == BigDecimal('-0.8')

      @one_minus.modulo(BigDecimal('0.8')).should == BigDecimal('0.6')
      @one.modulo(BigDecimal('-0.8')).should == BigDecimal('-0.6')

      @one_minus.modulo(BigDecimal('0.6')).should == BigDecimal('0.2')
      @one.modulo(BigDecimal('-0.6')).should == BigDecimal('-0.2')

      @one_minus.modulo(BigDecimal('0.5')).should == @zero
      @one.modulo(BigDecimal('-0.5')).should == @zero
      @one_minus.modulo(BigDecimal('-0.5')).should == @zero

      @one_minus.modulo(BigDecimal('0.4')).should == BigDecimal('0.2')
      @one.modulo(BigDecimal('-0.4')).should == BigDecimal('-0.2')

      @one_minus.modulo(BigDecimal('0.3')).should == BigDecimal('0.2')
      @one_minus.modulo(BigDecimal('0.2')).should == @zero
    end

    it "returns a [Float value] when the argument is Float" do
      @two.send(cmd, 2.0).should == 0.0
      @one.send(cmd, 2.0).should == 1.0
      res = @two.send(cmd, 5.0)
      res.kind_of?(Float).should == true
    end

    it "does NOT raise ZeroDivisionError if other is zero" do
      bd6543 = BigDecimal.new("6543.21")
      bd5667 = BigDecimal.new("5667.19")
      a = BigDecimal("1.0000000000000000000000000000000000000000005")
      b = BigDecimal("1.00000000000000000000000000000000000000000005")

      bd5667.send(cmd, 0).nan?.should == true
      if (mode != :exclude_float_zero)
        # MRI weird behavior. While #modulo returns NaN,
        # #divmod[1] raises an Error, so we guard it here.
        bd5667.send(cmd, 0.0).nan?.should == true
      end
      bd5667.send(cmd, BigDecimal("0")).nan?.should == true
      @zero.send(cmd, @zero).nan?.should == true
    end

    it "returns NaN if NaN is involved" do
      @nan.send(cmd, @nan).nan?.should == true
      @nan.send(cmd, @one).nan?.should == true
      @one.send(cmd, @nan).nan?.should == true
      @infinity.send(cmd, @nan).nan?.should == true
      @nan.send(cmd, @infinity).nan?.should == true
    end

    it "returns NaN if Infinity is involved" do
      @infinity.send(cmd, @infinity).nan?.should == true
      @infinity.send(cmd, @one).nan?.should == true
      @infinity.send(cmd, @mixed).nan?.should == true
      @infinity.send(cmd, @one_minus).nan?.should == true
      @infinity.send(cmd, @frac_1).nan?.should == true
      @one.send(cmd, @infinity).nan?.should == true

      @infinity_minus.send(cmd, @infinity_minus).nan?.should == true
      @infinity_minus.send(cmd, @one).nan?.should == true
      @one.send(cmd, @infinity_minus).nan?.should == true
      @frac_2.send(cmd, @infinity_minus).nan?.should == true

      @infinity.send(cmd, @infinity_minus).nan?.should == true
      @infinity_minus.send(cmd, @infinity).nan?.should == true
    end
    
    it "raises TypeError if the argument cannot be coerced to BigDecimal" do
      lambda {
        @one.send(cmd, '2')
      }.should raise_error(TypeError)
    end
  end
end

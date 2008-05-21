require 'bigdecimal'

shared :bigdecimal_quo do |cmd|

  describe "BigDecimal##{cmd}" do

    before(:each) do
      @one = BigDecimal("1")
      @zero = BigDecimal("0")
      @zero_plus = BigDecimal("+0")
      @zero_minus = BigDecimal("-0")
      @two = BigDecimal("2")
      @three = BigDecimal("3")
      @eleven = BigDecimal("11")
      @nan = BigDecimal("NaN")
      @infinity = BigDecimal("Infinity")
      @infinity_minus = BigDecimal("-Infinity")
      @one_minus = BigDecimal("-1")
      @frac_1 = BigDecimal("1E-99999")
      @frac_2 = BigDecimal("0.9E-99999")
    end

    it "returns a / b" do
      @two.send(cmd, @one).should == @two
      @one.send(cmd, @two).should == BigDecimal("0.5")
      @eleven.send(cmd, @three).should be_close(@three + (@two / @three), TOLERANCE)
      @one.send(cmd, @one_minus).should == @one_minus
      @one_minus.send(cmd, @one_minus).should == @one
      @frac_2.send(cmd, @frac_1).should == BigDecimal("0.9")
      @frac_1.send(cmd, @frac_1).should == @one
      (@one.send(cmd, BigDecimal('-2E5555'))).should == BigDecimal('-0.5E-5555')
      (@one.send(cmd, BigDecimal('2E-5555'))).should == BigDecimal('0.5E5555')
    end

    it "returns NaN if NaN is involved" do
      @one.send(cmd, @nan).nan?.should == true
      @nan.send(cmd ,@one).nan?.should == true
    end

    it "returns 0 if divided by Infinity" do
      @zero.send(cmd, @infinity).should == 0
      @frac_2.send(cmd, @infinity).should == 0
    end

    it "returns (+|-) Infinity if (+|-) Infinity divided by one" do
      @infinity_minus.send(cmd, @one).should == @infinity_minus
      @infinity.send(cmd, @one).should == @infinity
      @infinity_minus.send(cmd, @one_minus).should == @infinity
    end

    it "returns NaN if Infinity / ((+|-) Infinity)" do
      @infinity.send(cmd, @infinity_minus).nan?.should == true
      @infinity_minus.send(cmd, @infinity).nan?.should == true
    end

    it "returns (+|-) Infinity if divided by zero" do
      @one.send(cmd, @zero).should == @infinity
      @one.send(cmd, @zero_plus).should == @infinity
      @one.send(cmd, @zero_minus).should == @infinity_minus
    end

    it "returns NaN if zero is divided by zero" do
      @zero.send(cmd, @zero).nan?.should == true
      @zero_minus.send(cmd, @zero_plus).nan?.should == true
      @zero_plus.send(cmd, @zero_minus).nan?.should == true
    end

  end

end

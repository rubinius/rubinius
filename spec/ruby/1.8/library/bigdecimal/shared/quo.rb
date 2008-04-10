require 'bigdecimal'

shared :bigdecimal_quo do |cmd|

  describe "BigDecimal##{cmd}" do

    before(:each) do
      @one = BigDecimal("1")
      @zero = BigDecimal("0")
      @two = BigDecimal("2")
      @three = BigDecimal("3")
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
      @one.send(cmd, @one_minus).should == @one_minus
      @one_minus.send(cmd, @one_minus).should == @one
      @frac_2.send(cmd, @frac_1).should == BigDecimal("0.9")
      @frac_1.send(cmd, @frac_1).should == @one
    end

    it "returns NaN if NaN is involved" do
      @one.send(cmd, @nan).to_s.should == 'NaN'
      @nan.send(cmd ,@one).to_s.should == 'NaN'
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
      @infinity.send(cmd, @infinity_minus).to_s.should == "NaN"
      @infinity_minus.send(cmd, @infinity).to_s.should == 'NaN'
      # to_s needed because BigDecimal("NaN") never equals its self.
    end

  end

end

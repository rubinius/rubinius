require 'bigdecimal'

shared :bigdecimal_eql do |cmd|

  describe "BigDecimal##{cmd}" do

    before(:each) do
      @bg6543_21 = BigDecimal.new("6543.21")
      @bg5667_19 = BigDecimal.new("5667.19")
      @a = BigDecimal("1.0000000000000000000000000000000000000000005")
      @b = BigDecimal("1.00000000000000000000000000000000000000000005")
      @bigint = BigDecimal("1000.0")
      @nan = BigDecimal("NaN")
      @infinity = BigDecimal("Infinity")
      @infinity_minus = BigDecimal("-Infinity")
    end

    it "test for equality" do
      @bg6543_21.send(cmd, @bg6543_21).should == true
      @a.send(cmd, @a).should == true
      @a.send(cmd, @b).should == false
      @bg6543_21.send(cmd, @a).should == false
      @bigint.send(cmd, 1000).should == true
    end

    it "NaN is never equal to any number" do
      @nan.send(cmd, @nan).should_not == true
      @a.send(cmd, @nan).should_not == true
      @nan.send(cmd, @a).should_not == true
      @nan.send(cmd, @infinity).should_not == true
      @nan.send(cmd, @infinity_minus).should_not == true
      @infinity.send(cmd, @nan).should_not == true
      @infinity_minus.send(cmd, @nan).should_not == true
    end

    it "returns true for infinity values with the same sign" do
      @infinity.send(cmd, @infinity).should == true
      @infinity.send(cmd, BigDecimal("Infinity")).should == true
      BigDecimal("Infinity").send(cmd, @infinity).should == true

      @infinity_minus.send(cmd, @infinity_minus).should == true
      @infinity_minus.send(cmd, BigDecimal("-Infinity")).should == true
      BigDecimal("-Infinity").send(cmd, @infinity_minus).should == true
    end

    it "does not return true for infinity values with different signs" do
      @infinity.send(cmd, @infinity_minus).should_not == true
      @infinity_minus.send(cmd, @infinity).should_not == true
    end

    it "does not return true when ininite value compared to finite one" do
      @infinity.send(cmd, @a).should_not == true
      @infinity_minus.send(cmd, @a).should_not == true

      @a.send(cmd, @infinity).should_not == true
      @a.send(cmd, @infinity_minus).should_not == true
    end
  end
end

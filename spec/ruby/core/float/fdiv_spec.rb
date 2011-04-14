require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Float#fdiv" do
    before(:all) do
      @inf = 1/0.00
      @nan = 0/0.0
    end

    it "performs floating-point division between self and a Fixnum" do
      8.9.fdiv(7).should == 1.2714285714285716
    end

    it "performs floating-point division between self and a Bignum" do
      8.9.fdiv(9999999999999**9).should == 8.900000000008011e-117
    end

    it "performs floating-point division between self and a Float" do
      2827.22.fdiv(872.111111).should == 3.2418116961704433
    end

    it "returns NaN when the argument is NaN" do
      -1819.999999.fdiv(@nan).nan?.should be_true
      11109.1981271.fdiv(@nan).nan?.should be_true
    end

    it "returns Infinity when the argument is 0.0" do
      2827.22.fdiv(0.0).infinite?.should == 1
    end

    it "returns -Infinity when the argument is 0.0 and self is negative" do
      -48229.282.fdiv(0.0).infinite?.should == -1
    end

    it "returns Infinity when the argument is 0" do
      2827.22.fdiv(0).infinite?.should == 1
    end

    it "returns -Infinity when the argument is 0 and self is negative" do
      -48229.282.fdiv(0).infinite?.should == -1
    end

    it "returns 0.0 when the argument is Infinity" do
      47292.2821.fdiv(@inf).should == 0.0
    end

    it "returns -0.0 when the argument is -Infinity" do
      1.9999918.fdiv(-@inf).should == -0.0
    end

    it "performs floating-point division between self and a Rational" do
      74620.09.fdiv(Rational(2,3)).should == 111930.135
    end

    it "performs floating-point division between self and a Complex" do
      74620.09.fdiv(Complex(8,2)).should == Complex(
        8778.834117647059, -2194.7085294117646)
    end

    it "raises a TypeError when argument isn't numeric" do
      lambda { 27292.2.fdiv(mock('non-numeric')) }.should raise_error(TypeError)
    end

    it "raises an ArgumentError when passed multiple arguments" do
      lambda { 272.221.fdiv(6,0.2) }.should raise_error(ArgumentError)
    end
  end
end

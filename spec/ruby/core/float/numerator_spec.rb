require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Float#numerator" do
    before(:all) do
      @numbers = [
        29871.2722891,
        999.1**99.928888,
        -72628191273.22,
        29282.2827,
        -2927.00091,
        12.0,
        Float::MAX,
      ]
    end

    it "converts self to a Rational object then returns its numerator" do
      @numbers.each do |number|
        number.infinite?.should be_nil
        number.numerator.should == Rational(number).numerator
      end
    end

    it "returns 0 for 0.0" do
      0.0.numerator.should == 0
    end

    it "returns NaN for NaN" do
      nan = 0/0.0
      nan.nan?.should be_true
      nan.numerator.nan?.should be_true
    end

    it "returns Infinity for Infinity" do
      inf = 1/0.0
      inf.infinite?.should == 1
      inf.numerator.infinite?.should == 1
    end

    it "returns -Infinity for -Infinity" do
      inf = -(1/0.0)
      inf.infinite?.should == -1
      inf.numerator.infinite?.should == -1
    end

  end
end

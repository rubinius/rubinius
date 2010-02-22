require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Numeric#rationalize" do
    before(:all) do
      @numbers = [
        0,
        29871,
        99999999999999**99,
        -72628191273,
      ]
    end

    it "returns a Rational object" do
      @numbers.each do |number|
        number.rationalize.should be_an_instance_of(Rational)
      end
    end

    it "uses self as the numerator" do
      @numbers.each do |number|
        number.rationalize.numerator.should == number
      end
    end

    it "uses 1 as the denominator" do
      @numbers.each do |number|
        number.rationalize.denominator.should == 1
      end
    end
  end
end

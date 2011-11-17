require File.expand_path('../../../../spec_helper', __FILE__)

describe :numeric_arg, :shared => true do
  before(:each) do
    @numbers = [
      20,
      Rational(3, 4),
      bignum_value,
      infinity_value
    ]
  end

  it "returns 0 if positive" do
    @numbers.each do |number|
      number.send(@method).should == 0
    end
  end

  it "returns Pi if negative" do
    @numbers.each do |number|
      (0-number).send(@method).should == Math::PI
    end
  end
end

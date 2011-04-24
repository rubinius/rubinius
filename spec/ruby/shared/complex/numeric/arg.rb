require File.expand_path('../../../../spec_helper', __FILE__)

describe :numeric_arg, :shared => true do
  before(:each) do
    @numbers = [
      20,             # Integer
      398.72,         # Float
      Rational(3, 4), # Rational
      99999999**99, # Bignum
      Float::MAX * 2, # Infinity
    ]
  end

  it "returns 0 if positive" do
    @numbers.each do |number|
      number.send(@method).should == 0
    end
  end

  ruby_bug "#1715", "1.8.6.369" do
    it "returns NaN if self is NaN" do
      (-nan_value).send(@method).nan?.should be_true
      nan_value.send(@method).nan?.should be_true
    end
  end

  it "returns Pi if negative" do
    @numbers.each do |number|
      (0-number).send(@method).should == Math::PI
    end
  end

  # This was established in r23960
  it "returns Pi if -0.0" do
    (-0.0).send(@method).should == Math::PI
  end

  it "raises an ArgumentError if given any arguments" do
   @numbers.each do |number|
     lambda { number.send(@method, number) }.should raise_error(ArgumentError)
   end
  end
end

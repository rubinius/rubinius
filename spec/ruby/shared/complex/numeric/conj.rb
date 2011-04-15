require File.expand_path('../../../../spec_helper', __FILE__)

describe :numeric_conj, :shared => true do
  before(:each) do
    @numbers = [
      20,             # Integer
      398.72,         # Float
      Rational(3, 4), # Rational
      99999999**99, # Bignum
      Float::MAX * 2, # Infinity
      0/0.0           # NaN
    ]
  end

  it "returns self" do
    @numbers.each do |number|
      number.send(@method).to_s.should == number.to_s
    end
  end

  it "raises an ArgumentError if given any arguments" do
   @numbers.each do |number|
     lambda { number.send(@method, number) }.should raise_error(ArgumentError)
   end
  end
end

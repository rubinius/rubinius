require 'complex'

describe :numeric_conjugate, :shared => true do
  it "returns self" do
    20.send(@method).should == 20
    (-4.5).send(@method).should == -4.5
    bignum_value.send(@method).should == bignum_value
  end
end

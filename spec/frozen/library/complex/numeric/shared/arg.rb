require 'complex'

describe :numeric_arg, :shared => true do
  it "returns 0 if self is greater than or equal to 0" do
    0.send(@method).should == 0
    0.0.send(@method).should == 0
    (-0).send(@method).should == 0

    20.send(@method).should == 0
    4.5.send(@method).should == 0
    bignum_value.send(@method).should == 0
  end

  it "returns Math::PI if self is greater than or equal to 0" do
    (-20).send(@method).should == Math::PI
    (-4.5).send(@method).should == Math::PI
    (-bignum_value).send(@method).should == Math::PI
  end
end

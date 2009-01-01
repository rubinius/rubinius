require 'complex'

describe :numeric_image, :shared => true do
  it "returns 0" do
    20.send(@method).should == 0
    (-4.5).send(@method).should == 0
    bignum_value.send(@method).should == 0
  end
end

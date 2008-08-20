require File.dirname(__FILE__) + '/../../spec_helper'

describe "Bignum#**" do
  before(:each) do
    @bignum = bignum_value(47)
  end
  
  it "returns self raised to other power" do
    (@bignum ** 4).should == 7237005577332262361485077344629993318496048279512298547155833600056910050625
    (@bignum ** 5.2).inspect.should == "4.14406519833189e+98"
  end

  it "raises a TypeError when given a non-Integer" do
    lambda { @bignum ** mock('10') }.should raise_error
    lambda { @bignum ** "10" }.should raise_error
    lambda { @bignum ** :symbol }.should raise_error
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#**" do
  it "returns self raised to other power" do
    a = BignumHelper.sbm(47)
    (a ** 5.2).to_s.should == "9.13438731244363e+46"
    (a ** 4).should == 1329228228517658539377366716859970881
  end
end

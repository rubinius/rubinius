require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#&" do
  # 18446744073709551616 == 2**64
  # 36893488147419103232 == 2**65
  it "returns self bitwise AND other" do
    a = BignumHelper.sbm(5)
    (a & 3.4).should == 1
    (a & 52).should == 4
    (a & BignumHelper.sbm(9921)).should == 1073741825
    (18446744073709551616).&(1).should_be_close(0,0.00001)
    (18446744073709551616).&(-1).should == 18446744073709551616
    (36893488147419103232).&(-1).should == 36893488147419103232
    (18446744073709551616).&(18446744073709551616).should == 18446744073709551616
  end
end

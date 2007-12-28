require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#~" do
  it "returns self with each bit flipped" do
    (~BignumHelper.sbm(48)).should == -9223372036854775857
    (~(-BignumHelper.sbm(21))).should == 9223372036854775828
    (~BignumHelper.sbm(1)).should == -9223372036854775810
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#-@" do
  it "returns self as a negative value" do
    BignumHelper.sbm.send(:-@).should == -9223372036854775808
    (-BignumHelper.sbm).send(:-@).should == 9223372036854775808
    
    BignumHelper.sbm(921).send(:-@).should == -9223372036854776729
    (-BignumHelper.sbm(921).send(:-@)).should == 9223372036854776729
  end
end

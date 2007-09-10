require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#-@" do
  it "negates self" do
    BignumHelper.sbm.send(:-@).should == (-1073741824)
    BignumHelper.sbm(921).send(:-@).should == (-1073742745)
  end
end

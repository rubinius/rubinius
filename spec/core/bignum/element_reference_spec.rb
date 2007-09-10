require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#[]" do
  it "returns the nth bit in the binary representation of self" do
    a = BignumHelper.sbm(4996)
    a[2].should == 1
    a[9.2].should == 1
    a[21].should == 0
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#coerce" do
  it "returns [other, self] both as Bignum if other is an Integer" do
    a = BignumHelper.sbm
    a.coerce(2).should == [2, 1073741824]
    a.coerce(BignumHelper.sbm(701)).should == [1073742525, 1073741824]
  end
  
  it "returns [Bignum, Bignum] if other is a Bignum" do
    a = 0xffffffff.coerce(0xffffffff)
    a.should == [4294967295, 4294967295]
    a.collect { |i| i.class }.should == [Bignum, Bignum]
  end
end

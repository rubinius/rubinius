require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#divmod" do
  it "returns an [quotient, modulus] from dividing self by other" do
    a = BignumHelper.sbm(55)
    a.divmod(5).inspect.should == '[214748375, 4]'
    a.divmod(15.2).inspect.should == '[70640913, 1.40000005019339]'
    a.divmod(a + 9).inspect.should == '[0, 1073741879]'
  end
  
  it "raises ZeroDivisionError if other is zero and not a Float" do
    should_raise(ZeroDivisionError) { BignumHelper.sbm(2).divmod(0) }
  end
  
  it "raises FloatDomainError if other is zero and is a Float" do
    should_raise(FloatDomainError) { BignumHelper.sbm(9).divmod(0.0) }
  end
end

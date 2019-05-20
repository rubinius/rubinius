require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#untrusted?" do
  it "returns the untrusted status of an object" do
    o = mock('o')
    o.untrusted?.should be_false
    o.untrust
    o.untrusted?.should be_true
  end

  it "has no effect on nil, true, false" do
    a = nil
    b = true
    c = false
    a.untrust
    b.untrust
    c.untrust
    a.untrusted?.should be_false
    b.untrusted?.should be_false
    c.untrusted?.should be_false
  end

  it "has no effect on Fixnum" do
    1.untrust.should == 1
    1.untrusted?.should be_false
  end

  it "has no effect on Bignum" do
    value = bignum_value
    value.untrust.should == value
    value.untrusted?.should be_false
  end

  it "has no effect on a Float" do
    value = 42.0
    value.untrust.should == value
    value.untrusted?.should be_false
  end

  it "has no effect on a Symbol" do
    value = :a
    value.untrust.should == value
    value.untrusted?.should be_false
  end
end

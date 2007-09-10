require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Bignum#==" do
  it "should true if self has the same value as other" do
    a = BignumHelper.sbm(67)
    (a == a).should == true
    (a == 5.4).should == false
    (a == 121).should == false
  end

  it "calls 'other == self' if coercion fails" do
    class X; def ==(other); BignumHelper.sbm(123) == other; end; end

    (BignumHelper.sbm(120) == X.new).should == false
    (BignumHelper.sbm(123) == X.new).should == true
  end
end
